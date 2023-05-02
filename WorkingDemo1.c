#include <stdio.h>
#include <stdlib.h>
#include <sys/bios.h>
#include <sys/libwwc.h>
#include "Ground.h"
#include "Grapple_Boy256.h"
#include "Grapple_Boy4color-Walk.h"
#include "Grapple_Boy4color-L.h"
#include "Grapple_Boy4color-WalkL.h"
#include "LanceGuardF1_3.h"
#include "GBuplunge.h"
#include "GBrightlunge.h"
#include "GBdownlunge.h"
#include "GBleftlunge.h"
#include "GBslash1.h"
#include "GBslash2.h"
#include "GBslash3.h"

void init_sprite()
{
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
    } else {
	sprite_set_char(0, 0);
	sprite_set_char(1, 1);
    }
}
/* void clear_screen(BYTE screen, BOOL stripe)
{
    int i;

    for (i = 0; i < 32; i++) {
	screen_fill_char(screen, i, 0, 1, 32,
	    0 + (stripe ? (1 - i << CFSFT_PALETTE) : 0));
    }
} */
void subPlatformSet(int iScr ,int iX ,int iY ,int iWmax ,int iHmax ,int iPal,int iChn)
{
    int iW,iH;

    for(iH=0 ;iH<iHmax ;iH++)
    {
        for(iW=0 ;iW<iWmax ;iW++)
        {
             screen_fill_char(iScr ,iX+iW ,iY+iH, 1, 1 ,iChn | (iPal << CFSFT_PALETTE));
        }
    }
}
void subTextureSet(int iScr ,int iX ,int iY ,int iWmax ,int iHmax ,int iPal,int iChn)
{
    int iW,iH;

    for(iH=0 ;iH<iHmax ;iH++)
    {
        for(iW=0 ;iW<iWmax ;iW++)
        {
             screen_fill_char(iScr ,iX+iW ,iY+iH, 1, 1 ,(iPal << CFSFT_PALETTE));
        }
    }
}
void groundinit(){

    subTextureSet(SCREEN2 ,0 ,0 ,SCREEN_CHAR_WIDTH, SCREEN_CHAR_HEIGHT ,0 ,2);
    subPlatformSet(SCREEN1 ,0 ,10,SCREEN_CHAR_WIDTH, 8                  ,0,2);
    subTextureSet(SCREEN2 ,0 ,10,SCREEN_CHAR_WIDTH, 8                  ,0,2);
}
#include "title(new).fbm"
#define FONT_SPACE 0
#define FONT_BAR   (FONT_SPACE + 1)
#define FONT_BALL  (FONT_BAR   + 3)
#define FONT_TITLE (FONT_BALL  + 2)
#define FONT_FREE  (FONT_TITLE + 171)
/*----------------------------------------------------------------------
 * MENU
 *----------------------------------------------------------------------
 */
#define MENU_EXIT   0
#define MENU_START  1
#define MENU_OPTION 2

#define TITLE_X 4
#define TITLE_Y 4

void draw_title(BYTE screen)
{
    WORD font = FONT_TITLE;
    int dx;
    int dy;

    for (dy = 0; dy < title_height; dy++) {
	for (dx = 0; dx < title_width; dx++, font++) {
	    screen_fill_char(screen, TITLE_X + dx, TITLE_Y + dy, 1, 1, font);
	}
    }
}

void draw_string(BYTE screen, BYTE x, BYTE y, WORD font, unsigned char *s)
{
    BYTE data[8];
    WORD code;

    while ((code = *s++) != 0x00) {
	text_get_fontdata(code, data);
	font_set_monodata(font, 1, data);
	screen_set_char(screen, x++, y, 1, 1, &font);
	font++;
    }
}

void clear_screen(BYTE screen, BOOL stripe)
{
    int i;

    for (i = 0; i < 32; i++) {
	screen_fill_char(screen, i, 0, 1, 32,
	    FONT_SPACE + (stripe ? (1 - i % 2 << CFSFT_PALETTE) : 0));
    }
}
int pause()
{
    clear_screen(SCREEN2, FALSE);
    draw_string(SCREEN2, 11, 7, FONT_FREE, "PAUSE");
    draw_string(SCREEN2, 7, 9, FONT_FREE + 5, "START: Exit");
    draw_string(SCREEN2, 7, 10, FONT_FREE + 16, "OTHER: Restart");
    display_control(DCM_SCR1 | DCM_SCR2 | DCM_SCR2_WIN_INSIDE | DCM_SPR);
    switch (key_wait()) {
    	case KEY_START:
			return 0;
		/* case KEY_Y4
			return 2; */
    	default:
			return 1;
    }
}

void main(int argc, char *argv[]) {
	int i, sprite_cycle, direction, jump_height_potential, x_press, y_press , l_momentum, r_momentum, slash_anim, lunge, lunge_lag, lunge_anim, guard_anim, iJmp, iChn;
	BYTE x, y;
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
	x_press = 0; /* Tracks whether the player is pressing the X button. 0 = No and 1 = Yes */
	y_press = 0; /* Tracks whether the player is pressing the Y button. 0 = No and 1 = Yes */
	l_momentum = 0; /* Left momentum */
	r_momentum = 0; /* Right momentum */
	slash_anim = 0; /* Tracks the progress of the Sword Slash sprite animation */
	lunge = 1; /* Lunge counter. */
	lunge_lag = 0; /* Tracks how much the player must wait to regain lunge. */
	lunge_anim = 0; /* Tracks the progress of the Sword Lunge sprite animation */
	guard_anim = 0; /* Tracks the progress of the Lance Guard sprite animation */

	do {
		BYTE x2, y2, x3, y3;
		BYTE loop = TRUE;
		key_data = key_press_check();

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
			if((iChn==0)&(jump_height_potential==0)) /* If player is off the ground and no longer rising from a Jump, gravity is in effect */
			{
				y=y+2;
			}else{
			    if (key_data & KEY_A){ /* If the player jumps, they use the walk sprite. */
					if (direction == 0){
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
					}
					if (jump_height_potential<8){
						jump_height_potential++;
						iJmp = 3;
					}
					else{
						jump_height_potential=0;
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
				y=y-2;
			}
		}
		/* Check whether an X D-Pad button is pressed. */
		if (KEY_UP1 || KEY_RIGHT1 || KEY_DOWN1 || KEY_LEFT1){
			x_press=1;
		}
		/* Check whether a Y D-Pad button is pressed. */
		if (KEY_Y1 || KEY_Y2 || KEY_Y3 || KEY_Y4){
			y_press=1;
		}
		/* Decrements lunge_lag every frame. */
		if (lunge_lag > 0) {
			lunge_lag--;
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
			x--;
			if (key_data & KEY_B){ /* If player is in Run on ground or in fast movement in Freefall. */
				r_momentum=0;
				if (iChn!=0){
					if ((sprite_cycle%2)==0){ /* Every two sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
					}
				}
				if ((iChn != 0) || (l_momentum==1)){ /* If on ground and/or has momentum, the player moves fast.*/
					x = x - 2;
					l_momentum=1;
				}
			}else{ /* If player is in Walk or in slow movement in Freefall. */
				if (iChn!=0){
					if ((sprite_cycle%3)==0){ /* Every three sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
					}
				}
				if ((iChn == 0) & (l_momentum==1)){ /* If player is in air and has momentum, they maintain high speed */
					x = x - 2;
				}
			}
		}

		/* Right Movement */
		if (key_data & KEY_RIGHT1){
			direction = 1; /* Refers to player facing Right. */
			x++;
			if (key_data & KEY_B){ /* If player is in Run on ground or in fast movement in Freefall. */
				l_momentum=0;
				if (iChn!=0){
					if ((sprite_cycle%2)==0){ /* Every two sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4color);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
					}
				}
				if ((iChn != 0) || (r_momentum==1)){ /* If on ground and/or has momentum, the player moves fast.*/
					x = x + 2;
					r_momentum=1;
				}
			}else{ /* If player is in Walk or in slow movement in Freefall. */
				if (iChn!=0){
					if ((sprite_cycle%3)==0){ /* Every three sprite cycles, the sprite alternates. */
						font_set_colordata(4, 16, bmp_Grapple_Boy4color);
					}
					else{
						font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
					}
				}
				if ((iChn == 0) & (r_momentum==1)){ /* If player is in air and has momentum, they maintain high speed */
					x = x + 2;
				}
			}
		}
		
		/* High-Frequency Sword attacks */
		if (key_data & KEY_B){
			if ((lunge == 1) & (y_press == 1) & (lunge_lag == 0)) { 
				lunge=0;
				lunge_lag=50;
				if (key_data & KEY_Y1){ /* If player presses Y1 (Up), they lunge in said direction. */
					y = y - 32;
					font_set_colordata(4, 16, bmp_GBuplunge);
				}
				else if (key_data & KEY_Y2){ /* If player presses Y2 (Right), they lunge in said direction. */
					direction = 1;
					x = x + 30;
					if (iChn==0){ /* If player is in Freefall, they resist gravity enough to move laterally straight */
						y = y - 2;
					}
					font_set_colordata(4, 16, bmp_GBrightlunge);
				}
				else if (key_data & KEY_Y3){ /* If player presses Y3 (Down), they lunge in said direction. */
					y = y + 30;
					font_set_colordata(4, 16, bmp_GBdownlunge);
				}
				else if (key_data & KEY_Y4){ /* If player presses Y4 (Left), they lunge in said direction. */
					direction = 0;
					x = x - 30;
					if (iChn==0){ /* If player is in Freefall, they resist gravity enough to move laterally straight */
						y = y - 2;
					}
					font_set_colordata(4, 16, bmp_GBleftlunge);
				}
			}
			else/* if (x_press == 0) */{
		    	if (slash_anim < 4){
					slash_anim++;
					font_set_colordata(6,72, bmp_GBslash1);
				}
				else if (slash_anim < 8){
					slash_anim++;
					font_set_colordata(6,72, bmp_GBslash2);
				}
				else if (slash_anim < 12){
					slash_anim++;
					font_set_colordata(6,72, bmp_GBslash3);
				}
				else {
					slash_anim = 0;
					font_set_colordata(4,16, bmp_Grapple_Boy4colorWalk);
				}
			}
		}

		/* Charging Lance attacks */
		/* 
		if (key_data & KEY_B){
			if (key_data & KEY_Y1){

			}
			else if (key_data & KEY_Y2){

			}
			else if (key_data & KEY_Y3){

			}
			else if (key_data & KEY_Y4){

			}
		}
		
		
		*/

		x_press = 0; /* Resets the check for an X button press. */
		y_press = 0; /* Resets the check for a Y button press. */
		x2 = x + 8;
		y2 = y + 8;
		x3 = x + 16;
		y3 = y + 16;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);
		/* call these only when needed */
		sprite_set_location(4, x3, y);
		sprite_set_location(5, x, y3);
		sprite_set_location(6, x3, y2);
		sprite_set_location(7, x2, y3);
		sprite_set_location(8, x3, y3);
		if (key_data & KEY_DOWN1){
			sys_wait(1);
		}
		if (key_data & KEY_START) {
	    	loop = pause();
			if (loop == 0) {
        		break;  /* exit the loop if loop is 0 */
   		 	}
	    	display_control(DCM_SCR1 | DCM_SPR);
		}
	} while(loop);
	return;
}
