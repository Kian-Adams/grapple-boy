#include <stdio.h>
#include <stdlib.h>
#include <sys/bios.h>
#include <sys/libwwc.h>
#include "Ground.h"
#include "Grapple_Boy256.h"
#include "Grapple_Boy4color-Walk.h"
#include "Grapple_Boy4color-L.h"
#include "Grapple_Boy4color-WalkL.h"
#include "Hover1.h"
#include "Hover2.h"
#include "Hover1L.h"
#include "Hover2L.h"
#include "Slide.h"
#include "SlideL.h"
#include "SlideJump.h"
#include "SlideJumpL.h"
#include "Lunge.h"
#include "LungeL.h"
#include "title.fbm"


void init_sprite() {
    if (wwc_get_hardarch() == HARDARCH_WSC) {
	int i;
	wwc_set_color_mode(COLOR_MODE_4COLOR);
	for (i = 0; i < 15; i++) {
	    wwc_palette_set_color(i, 0, 0x0000);
	    wwc_palette_set_color(i, 1, 0x0666);
	    wwc_palette_set_color(i, 2, 0x0aaa);
	    wwc_palette_set_color(i, 3, 0x0fff);
		wwc_palette_set_color(i, 4, 0x0999);
	}

	sprite_set_char(0, 0 + (1 << 9));
	wwc_palette_set_color(1 + 8, 3, 0x0ff0);

	sprite_set_char(1, 1 + (2 << 9));
	wwc_palette_set_color(2 + 8, 3, 0x0f00);
    }
	else {
	sprite_set_char(0, 0);
	sprite_set_char(1, 1);
    }
}

void subPlatformSet(int iScr ,int iX ,int iY ,int iWmax ,int iHmax ,int iPal,int iChn) {
    int iW,iH;

    for(iH=0 ;iH<iHmax ;iH++)
    {
        for(iW=0 ;iW<iWmax ;iW++)
        {
             screen_fill_char(iScr ,iX+iW ,iY+iH, 1, 1 ,iChn | (iPal << CFSFT_PALETTE));
        }
    }
}

void subTextureSet(int iScr ,int iX ,int iY ,int iWmax ,int iHmax ,int iPal,int iChn) {
    int iW,iH;

    for(iH=0 ;iH<iHmax ;iH++) {
        for(iW=0 ;iW<iWmax ;iW++) {
             screen_fill_char(iScr ,iX+iW ,iY+iH, 1, 1 ,(iPal << CFSFT_PALETTE));
        }
    }
}

void groundinit() {
    subTextureSet(SCREEN2, 0, 0, SCREEN_CHAR_WIDTH, SCREEN_CHAR_HEIGHT, 0, 2);
    subPlatformSet(SCREEN1, 0, 10, SCREEN_CHAR_WIDTH, 8,0 , 2);
    subTextureSet(SCREEN2, 0, 10, SCREEN_CHAR_WIDTH, 8, 0, 2);
}

#define FONT_SPACE 0
#define FONT_BAR   (FONT_SPACE + 1)
#define FONT_BALL  (FONT_BAR   + 3)
#define FONT_TITLE (FONT_BALL  + 2)
#define FONT_FREE  (FONT_TITLE + 171)

void draw_string(BYTE screen, BYTE x, BYTE y, WORD font, unsigned char *s) {
    BYTE data[8];
    WORD code;

    while ((code = *s++) != 0x00) {
	text_get_fontdata(code, data);
	font_set_monodata(font, 1, data);
	screen_set_char(screen, x++, y, 1, 1, &font);
	font++;
    }

	/*
 	ARG 1- the screen you want it on
 	ARG 2- the x coordinate you want the text to appear. Goes from the left to the right of the
 	screen (low number starts at left and ranges from 0 to 27)
 	ARG 3- the y coordinate you want the text to appear. Goes from top to bottom of the screen
 	(low number starts at top and ranges from 0 to 17)
 	ARG 4- the number of previous characters you want the text within the function to follow 
	(to display correctly, it must be the total number of previous characters).
 	ARG 5- the text you want displayed
 	*/
}

void clear_screen(BYTE screen, BOOL stripe) {
    int i;

    for (i = 0; i < 32; i++) {
		screen_fill_char(screen, i, 0, 1, 32,
		FONT_SPACE + (stripe ? (1 - i % 2 << CFSFT_PALETTE) : 0));
    }
}

void init_screen() {

    font_set_colordata(FONT_TITLE, 171, title);
    screen_set_scroll(SCREEN1, 0, 0);
    screen_set_scroll(SCREEN2, 0, 0);
}

void init_sound() {
    static unsigned char kukei_wave[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    sound_init();			
    sound_set_wave(0, kukei_wave);
    sound_set_wave(1, kukei_wave);
    sound_set_output(0x0f);		/* SND_OUT_CTRL   */
    sound_set_channel(0x01);		/* SND_CH_CTRL    */
}

void exit() {
    bios_exit();
}

BOOL pause() {
    clear_screen(SCREEN2, FALSE);
    draw_string(SCREEN2, 11, 7, FONT_FREE, "PAUSE");
    draw_string(SCREEN2, 7, 9, FONT_FREE + 5, "START: Exit");
    draw_string(SCREEN2, 7, 10, FONT_FREE + 16, "OTHER: Unpause");
    display_control(DCM_SCR1 | DCM_SCR2 | DCM_SCR2_WIN_INSIDE | DCM_SPR);
	sound_set_volume(0, 0);
	switch (key_wait()) {
    	case KEY_START:
			return FALSE;
    	default:
			return TRUE;
    }
}
 
/*----------------------------------------------------------------------
 * GAMEPLAY FUNCTIONS
 *----------------------------------------------------------------------
 */

/* Tracks whether the player is pressing the X buttons. */

BOOL x2x4_press(WORD key_data) { 
	return ((key_data & KEY_RIGHT1) || (key_data & KEY_LEFT1));
}

/* Tracks whether the player is pressing the Y buttons. */
BOOL y_press(WORD key_data) {
	return ((key_data & KEY_Y1) || (key_data & KEY_Y2) || (key_data & KEY_Y3) || (key_data & KEY_Y4));
}

BOOL b_press(WORD key_data) {
	return (key_data & KEY_B);
}

/* Tracks current ability equipped. */
int ability_set(WORD key_data) {
	if (key_data & KEY_Y1) { /* Toggle Hover */
		return 1;
	}
	else if (key_data & KEY_Y2) { /* Toggle Lunge */
		return 2;
	}
	else if (key_data & KEY_Y3) { /* Toggle Time Slow */
		return 3;
	}
	else if (key_data & KEY_Y4) { /* Toggle Slide */
		return 4;
	}
}

/* The game's playing function */
BOOL play() {
	int i, sprite_cycle, direction, jump_height_potential, current_ability,
	l_momentum, r_momentum, l_slide, r_slide, l_slide_jump,	r_slide_jump, slash_anim, 
	lunge, lunge_lag, lunge_anim, guard_anim, iJmp, iChn;
	BYTE x, y, x2, y2;
	BYTE loop;
	WORD key_data;
	init_sprite();

	/* display off */
	display_control(0x0000);

	/* color setting */
	lcd_set_color(0x7640, 0xfb98);
	palette_set_color(0, 0x7520);
	palette_set_color(13, 0x7520);

	/* screen setting */
	clear_screen(SCREEN1, TRUE);
	groundinit();
	font_set_colordata(0, Ground_width * Ground_height, bmp_clear);
	font_set_colordata(2, Ground_width * Ground_height, bmp_Ground);
	screen_fill_char(SCREEN2, 0, 0, 32, 32, 0);
	screen_set_scroll(SCREEN1, 0, 0);


	/* sprite setting */
	font_set_colordata(4, 16, bmp_Grapple_Boy4color);
	for(i = 0; i < 128; i++)
		sprite_set_char_location(i, 0, 0xf8, 0xf8);
	sprite_set_range(0, 128);	

	/* display on */
	display_control(DCM_SCR1 | DCM_SPR);

	for(i = 0; i < 32; i++) {
		WORD chr;

		chr = i & 3;
		screen_fill_char(SCREEN2, 0, i, 32, 1, chr);
	}

	for(i = 0; i < 4; i++) {
		WORD chr;

		chr = (7 << 9) | (i + 4);
		sprite_set_char(i, chr);
	}

	x = 0; /* Player x position. Incrementing x results in moving Right whilst decrementing x results in moving Left. */
	y = 0; /* Player y position. Incrementing y results in moving Down whilst decrementing y results in moving Up. */
	sprite_cycle = 0; /* Sprite cycle. Used in alternating sprites per a certain amount of frames. */
	direction = 0; /* Direction the player is facing. 0 = Left, and 1 = Right. */
	iJmp = 0;	/* Jump counter. */
	iChn = 0; /* If iChn == 0, then player is in Freefall. */
	i = 0; /* Variable for incrementing the sprite cycle. */
	jump_height_potential = 0; /* Jump height potential. The longer A is held, the more k is incremented. Max Jump height is 8 units. */
	current_ability = 1; /* Tracks the current ability equipped. */
	l_momentum = 0; /* Left momentum */
	r_momentum = 0; /* Right momentum */
	l_slide = 0;
	r_slide = 0;
	l_slide_jump = 0;
	r_slide_jump = 0;
	slash_anim = 0; /* Tracks the progress of the Sword Slash sprite animation */
	lunge = 1; /* Lunge counter. */
	lunge_lag = 0; /* Tracks how much the player must wait to regain lunge. */
	lunge_anim = 0; /* Tracks the progress of the Sword Lunge sprite animation */
	guard_anim = 0; /* Tracks the progress of the Lance Guard sprite animation */
	loop = TRUE;

	do {
		/*sound_set_pitch(0, 0xBE);
		sound_set_volume(1, 0x88);*/
		key_data = key_press_check();
		
		if (y_press(key_data)) {
			current_ability = ability_set(key_data);
		}

		sys_wait(1);
		i++;
		if (i == 11){ /* The max of i is 10, which is reached after 10 frames. */
			i = 0;
		}
		if (i == 0) { /* The sprite cycle increments every 10 frames. */
			sprite_cycle++;
		}

		if(iJmp==0){	
			iChn=screen_get_char1(SCREEN1 , x/8 , (y+16)/8) & CFM_FONT;
			if((iChn==0) & (jump_height_potential==0)) /* If player is off the ground and no longer rising from a Jump, gravity is in effect */
			{
				y = y + 2;
				if ((lunge_lag > 25) & (iChn == 0)) {
					y = y - 2;
				}
			}else{
			    if (key_data & KEY_A){ /* If the player jumps, they use the walk sprite. */
					
					sound_set_pitch(0, 1010);
    				sound_set_volume(0, 0x88);
					if (direction == 0){
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
					}
					if (jump_height_potential < 8){
						jump_height_potential++;
						iJmp = 3;
						if (jump_height_potential > 3) {
						sound_set_volume(0, 0x44);
						}
					}
					else{
						jump_height_potential=0;
						sound_set_volume(0, 0);
					}
				}
				else{
					jump_height_potential=0;
					if (iChn != 0){
						l_momentum=0;
						r_momentum=0;
						lunge=1;
					}
				}
			}
		}
		else{		
			iJmp--;		
			if (y>0){
				y = y - 2;
				if ((l_slide_jump == 1) & (direction == 0)) {
					font_set_colordata(4, 16, bmp_SlideJumpL);
					x = x - 3;
					if (i%2 == 0) { /* rise by 1 every 2 frames while in the jump */
						y = y - 1;
					}
					sound_set_pitch(0, 1000);
    				sound_set_volume(0, 0x98);
				}
				if ((r_slide_jump == 1) & (direction == 1)) {
					font_set_colordata(4, 16, bmp_SlideJump);
					x = x + 3;
					if (i%2 == 0) { /* rise by 1 every 2 frames while in the jump */
						y = y - 1;
					}
					sound_set_pitch(0, 1000);
    				sound_set_volume(0, 0x98);
				}
			}
		}
		
		if (iChn != 0) {
			sound_set_volume(0, 0);
		}


		/* Other Ability code */

		

		/* Slide */
		/* If player is on the ground, they cannot use the Slide Jump. Can only be in effect after a Slide Jump landing. */
		if (iChn != 0) {
			l_slide_jump = 0;
			r_slide_jump = 0;
		}
		/* Slide counter is reset to 0 if in air, if not moving laterally, or if B is not pressed. */
		if ((iChn == 0) || (x2x4_press(key_data) == FALSE) || (b_press(key_data) == FALSE)) {
			l_slide = 0;
			r_slide = 0;
		}


		/* If player is not moving laterally on the ground, they use the still sprite. */
		if (((key_data & KEY_LEFT1) != TRUE) & ((key_data & KEY_RIGHT1) != TRUE) & (iChn != 0)){
			if (direction == 0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
			}
			else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4color);
			}	
		}

		/* Left Movement */
		if (key_data & KEY_LEFT1){
			direction = 0; /* Refers to player facing Left. Note: Player starts the game facing Left as well. */
			if (lunge_lag <= 25) {
				x--;
			}
			/* If player is in Walk or in slow movement in Freefall. */
			if ((key_data & KEY_B) != TRUE){
				if (iChn != 0){
					if ((sprite_cycle % 3) == 0){ /* Every three sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
						sound_set_pitch(0, 1111);
						sound_set_volume(0, 0x88);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
						sound_set_volume(0, 0);
					}
				}
				if ((iChn == 0) & (l_momentum == 1) & (current_ability != 4)){ /* If player is in air and has momentum, they maintain high speed */
					x = x - 1;
				}
			}
		}

		/* Right Movement */
		if (key_data & KEY_RIGHT1){
			direction = 1; /* Refers to player facing Right. */
			if (lunge_lag <= 25){
				x++;
			}
			/* If player is in Walk or in slow movement in Freefall. */
			if ((key_data & KEY_B) != TRUE){
				if (iChn != 0){
					if ((sprite_cycle % 3) == 0){ /* Every three sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4color);
						sound_set_pitch(0, 1111);
						sound_set_volume(0, 0x88);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
						sound_set_volume(0, 0);
					}
				}
				if ((iChn == 0) & (r_momentum==1) & (current_ability != 4)){ /* If player is in air and has momentum, they maintain high speed */
					x = x + 1;
				}
			}
		}

		/* Abilities */
		if (key_data & KEY_B){
			switch(current_ability){
				case 1: /* Hover */
					if ((iChn == 0) & (jump_height_potential == 0)){ /* FOR LATER: Make it so that it cannot activate mid-jump if b isnt pressed beforehand*/
						y = y - 2; /* gravity is counteracted*/
						if (i%4==0) { /* Gravity is not counteracted every 4 frames */
							y = y + 1;
						}
						if (key_data & KEY_LEFT1) { /* increased lateral movement */
							x = x - 1;
						}
						if (key_data & KEY_RIGHT1) {
							x = x + 1;
						}
						if ((sprite_cycle % 3) == 0) { /* Every three sprite cycles, the sprite alternates. */
							if (direction == 0) {
								font_set_colordata(4, 16, bmp_Hover1L);
							}
							else {
								font_set_colordata(4, 16, bmp_Hover1);
							}
							sound_set_pitch(0, 0110);
							sound_set_volume(0, 0x88);
						}
						else {
							if (direction == 0) {
								font_set_colordata(4, 16, bmp_Hover2L);
							}
							else {
								font_set_colordata(4, 16, bmp_Hover2);
							}
							sound_set_volume(0, 0);
						}
					}
					else if (jump_height_potential != 0){
						y = y - 1; /* jump speed increased while maintaining jump height */
						if (jump_height_potential > 4) {
							jump_height_potential++;
						}
					}
					break;
				case 2: /* Lunge */
					if ((lunge == 1) & ((x2x4_press(key_data) == TRUE) ) & (lunge_lag == 0)) { 
						lunge = 0;
						lunge_lag = 50;
						jump_height_potential = 8;
						if (key_data & KEY_RIGHT1){ /* If player presses X2 (Right), they lunge in said direction. */
							direction = 1;
							x = x + 30;
							if (iChn == 0){ /* If player is in Freefall, they resist gravity enough to move laterally straight */
								y = y - 2;
							}
						}
						else if (key_data & KEY_LEFT1){ /* If player presses X4 (Left), they lunge in said direction. */
							direction = 0;
							x = x - 30;
							if (iChn == 0){ /* If player is in Freefall, they resist gravity enough to move laterally straight */
								y = y - 2;
							}
						}
					}
					break;
				case 3: /* Time Slow */
					sys_wait(1);
					break;
				case 4: /* Slide */
					if (key_data & KEY_LEFT1){  /* in the process of making a limit to sliding */
						if ((l_slide < 40) & (iChn != 0)){
							l_slide++;
						}
						r_slide = 0;
						r_momentum=0;
						if (iChn != 0){
							if (l_slide == 40) {
								l_slide_jump = 1;
								font_set_colordata(4, 16, bmp_SlideL);
								sound_set_volume(0, 0x24);
							}
							else {
								if ((sprite_cycle % 2) == 0){ /* Every two sprite cycles, the sprite alternates. */
									font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
									sound_set_pitch(0, 1111);
									sound_set_volume(0, 0x88);
								}
								else {
									font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
									sound_set_volume(0, 0);
								}
							}
						}
						if ((iChn != 0) || (l_momentum == 1)){ /* If on ground and/or has momentum, the player moves fast.*/
							x = x - 1;
							l_momentum=1;
						}
					}
					if (key_data & KEY_RIGHT1){
						if ((r_slide < 40) & (iChn != 0)){
							r_slide++;
						}
						l_slide = 0;
						l_momentum = 0;
						if (iChn != 0){
							if (r_slide == 40) {
								r_slide_jump = 1;
								font_set_colordata(4, 16, bmp_Slide);
								sound_set_volume(0, 0x24);
							}
							else {
								if ((sprite_cycle % 2) == 0){ /* Every two sprite cycles, the sprite alternates. */
									font_set_colordata(4, 16, bmp_Grapple_Boy4color);
									sound_set_pitch(0, 1111);
									sound_set_volume(0, 0x88);
								}
								else {
									font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
									sound_set_volume(0, 0);
								}
							}
						}
						if ((iChn != 0) || (r_momentum==1)){ /* If on ground and/or has momentum, the player moves fast.*/
							x = x + 1;
							r_momentum=1;
						}
					}
					break;
				}
			}

			/* Lunge */
		/* Decrements lunge_lag every frame. */
		if (lunge_lag > 0) {
			if (direction == 1) {
				font_set_colordata(4, 16, bmp_Lunge);
			}
			else {
				font_set_colordata(4, 16, bmp_LungeL);
			}
			sound_set_pitch(0, 11000);
    		sound_set_volume(0, 0x22);
			lunge_lag--;
		}

		x2 = x + 8;
		y2 = y + 8;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);

		if (key_data & KEY_START) {
	    	loop = pause();
			if (loop == FALSE) {
        		break;  /* Exit the loop if START is pressed. */	
   		 	}
			display_control(DCM_SCR1 | DCM_SPR);
		}
	} while(loop); /* Exit the loop if loop is FALSE */
	return FALSE;
}

/*----------------------------------------------------------------------
 * MENUS
 *----------------------------------------------------------------------*/

#define MENU_EXIT    0
#define MENU_START   1
#define MENU_OPTION1 2
#define MENU_OPTION2 3

#define TITLE_X 4
#define TITLE_Y 4

void draw_title(BYTE screen) {
    WORD font = FONT_TITLE;
    int dx;
    int dy;

    for (dy = 0; dy < title_height; dy++) {
		for (dx = 0; dx < title_width; dx++, font++) {
	    screen_fill_char(screen, TITLE_X + dx, TITLE_Y + dy, 1, 1, font);
		}
    }
}

void y_controls_guide() {
	display_control(DCM_SCR1);
	clear_screen(SCREEN1, TRUE);
	draw_string(SCREEN1, 1, 1 , FONT_FREE      , "Y-CONTROLS (ABILITIES)");
	draw_string(SCREEN1, 1, 4 , FONT_FREE + 22 , "Y1 - Hover: Low gravity,");
	draw_string(SCREEN1, 1, 5 , FONT_FREE + 46 , "high lateral air movement");
	draw_string(SCREEN1, 1, 7 , FONT_FREE + 71 , "Y2 - Lunge: Instantly move");
	draw_string(SCREEN1, 1, 8 , FONT_FREE + 97 , "to either left or right");
	draw_string(SCREEN1, 1, 10, FONT_FREE + 120, "Y3 - Time Slow: Halve the");
	draw_string(SCREEN1, 1, 11, FONT_FREE + 145, "passage of time for easy");
	draw_string(SCREEN1, 1, 12, FONT_FREE + 169, "movements");
	draw_string(SCREEN1, 1, 14, FONT_FREE + 178, "Y4 - Slide: Run instead of");
	draw_string(SCREEN1, 1, 15, FONT_FREE + 204, "Walk, and after a while,");
	draw_string(SCREEN1, 1, 16, FONT_FREE + 228, "Slide and jump high");

	switch (key_wait()) {
		case KEY_A:
			break;
	}
}

void controls_guide() {

	display_control(DCM_SCR1);
	clear_screen(SCREEN1, TRUE);
	draw_string(SCREEN1, 1, 1 , FONT_FREE      , "IN-GAME CONTROLS");
	draw_string(SCREEN1, 1, 4 , FONT_FREE + 16 , "A - Jump");
	draw_string(SCREEN1, 1, 6 , FONT_FREE + 24 , "B - Use Ability");
	draw_string(SCREEN1, 1, 8 , FONT_FREE + 39 , "X2/X4 - Move Right/Left");
	draw_string(SCREEN1, 1, 10, FONT_FREE + 62 , "Y D-Pad - Toggle Abilities");
	draw_string(SCREEN1, 1, 12, FONT_FREE + 88 , "START - Pause");
	draw_string(SCREEN1, 1, 14, FONT_FREE + 101 , "Be next to Wall - Grapple");
	draw_string(SCREEN1, 1, 16, FONT_FREE + 126, "For ability info, press Y.");
	
	switch (key_wait()) {

		case KEY_Y1:
		 	y_controls_guide();
			break;
		case KEY_Y2:
			y_controls_guide();
			break;
		case KEY_Y3:
			 y_controls_guide();
			break;
		case KEY_Y4:
			y_controls_guide();
			break;
	}
}

void information() {

	display_control(DCM_SCR1);
	clear_screen(SCREEN1, TRUE);
	draw_string(SCREEN1, 1, 4 , FONT_FREE      , "This platformer is called");
	draw_string(SCREEN1, 1, 5 , FONT_FREE + 26 , "Grapple Boy. It was");
	draw_string(SCREEN1, 1, 6 , FONT_FREE + 45 , "created by Gavin Ramins,");
	draw_string(SCREEN1, 1, 7 , FONT_FREE + 69 , "Darren Bonifacio, and");
	draw_string(SCREEN1, 1, 8 , FONT_FREE + 90 , "Kian Adams for the MATES");
	draw_string(SCREEN1, 1, 9 , FONT_FREE + 115, "Senior CS Capstone class");
	draw_string(SCREEN1, 1, 10, FONT_FREE + 140, "in Spring 2023. Thanks to");	
	draw_string(SCREEN1, 1, 11, FONT_FREE + 166, "Dr. Bixler for giving us");	
	draw_string(SCREEN1, 1, 12, FONT_FREE + 191, "this unique opportunity.");	

	switch (key_wait()) {
		case KEY_A:
			break;
	}
}

void level_menu() {

	display_control(DCM_SCR1);
	clear_screen(SCREEN1, TRUE);
	draw_string(SCREEN1, 5, 4 , FONT_FREE     , "Y1: Level 1"); /* add sandbox mode */
	draw_string(SCREEN1, 5, 7 , FONT_FREE + 11, "Y2: Level 2");
	draw_string(SCREEN1, 5, 10, FONT_FREE + 22, "Y3: Level 3");
	draw_string(SCREEN1, 5, 13, FONT_FREE + 33, "OTHER: Exit");

	switch (key_wait()) {
		case KEY_Y1:
			play();
			break;
		case KEY_Y2:
			play();
			break;
		case KEY_Y3:
			play();
			break;
	}
}

int main_menu() {

	display_control(DCM_SCR1);
	clear_screen(SCREEN1, TRUE);
	draw_string(SCREEN1, 5, 4 , FONT_FREE     , "START: Levels");
	draw_string(SCREEN1, 5, 7 , FONT_FREE + 13, "A: Controls Guide");
	draw_string(SCREEN1, 5, 10, FONT_FREE + 30, "B: Information");
	draw_string(SCREEN1, 5, 13, FONT_FREE + 44, "OTHER: Exit");

	switch (key_wait()) {
		case KEY_START:
			return MENU_START;
			break;
		case KEY_A:
			return MENU_OPTION1;
			break;
		case KEY_B:
			return MENU_OPTION2;
			break;
    }
}

void main_screen() {

	switch(main_menu()) {
		case MENU_START:
			level_menu();
			sound_set_volume(0, 0);
			main_screen();
			break;
		case MENU_OPTION1:
			controls_guide();
			main_screen();
			break;
		case MENU_OPTION2:
			information();
			main_screen();
			break;
	}
}


int start_menu() {
	
	init_sprite();

	/* display off */
	display_control(0x0000);

	/* color setting */
	lcd_set_color(0x7640, 0xfb98);
	palette_set_color(0, 0x7520);
	palette_set_color(13, 0x7520);

	/* screen setting */
	
	screen_fill_char(SCREEN2, 0, 0, 32, 32, 0);
	screen_set_scroll(SCREEN1, 0, 0);

	clear_screen(SCREEN1, TRUE);
    display_control(DCM_SCR1);
    draw_title(SCREEN1);
    draw_string(SCREEN1, 7, 15, FONT_FREE, "PRESS ANY KEY");

    switch (key_wait()) {
    	case KEY_START:
			return MENU_EXIT;
    	default:
			return MENU_START;
    }
}

void start() {

	switch (start_menu()) {
		case MENU_START:
			main_screen();
			break;
		case MENU_EXIT:
			exit();
			break;
	}
}

void opening() {

    int x = 32;
    int y = 32;
    int width = title_width * 8;
    int height = title_height * 8;
    int i;

    display_control(DCM_SCR2 | DCM_SCR2_WIN_INSIDE);
    screen2_set_window(112, y, 0, height);
    draw_title(SCREEN2);
    for (i = 0; i <= width / 2; i++) {	
		screen2_set_window(x, y, i * 2, height);
		sound_set_volume(0, 0x26);
		if (i == 20) {
			sound_set_pitch(0, 1100);
		}
		else if (i == 40) {
			sound_set_pitch(0, 1001);
		}
		else if (i == 58) {
			sound_set_pitch(0, 1111);
		}
		sys_wait(1);
		sound_set_volume(0, 0);
    }
}

/*----------------------------------------------------------------------
 * MAIN FUNCTION
 *----------------------------------------------------------------------*/

void main(int argc, char *argv[]) {

	while (TRUE) {
		srand(sys_get_tick_count());
    	init_screen();
   		init_sound();
		opening();
		start();
	}
}
