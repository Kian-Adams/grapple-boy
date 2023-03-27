#include <stdio.h>
#include <stdlib.h>
#include <sys/bios.h>
#include <sys/libwwc.h>
#include "Ground.h"
#include "Grapple_Boy256.h"
#include "Grapple_Boy4color-Walk.h"
#include "Grapple_Boy4color-L.h"
#include "Grapple_Boy4color-WalkL.h"
#define JMP_ST 22
#define JMP_ED 0
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
void clear_screen(BYTE screen, BOOL stripe)
{
    int i;

    for (i = 0; i < 32; i++) {
	screen_fill_char(screen, i, 0, 1, 32,
	    0 + (stripe ? (1 - i << CFSFT_PALETTE) : 0));
    }
}
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
void main(int argc, char *argv[]) {
	int i, j, k, yPress, inJump, lMomentum, rMomentum, lunge, iJmp, iChn;
	BYTE x, y;
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

	x = 0;
	y = 0;
	iJmp=0;	/* max jump height???? */
	iChn=0; /* If iChn == 0, then player is in Freefall. */
	i=0; /* Variable for alternating between walking sprites. */
	j=0; /* Max Jump height. */
	k=0; /* Jump units. The longer A is held, the more k is incremented. Max Jump units is 7. */
	yPress=0;
	inJump=0;
	lMomentum=0;
	rMomentum=0;
	lunge=1;

	do {
		BYTE x2, y2;

		sys_wait(1);
		i++;
		if (i == 3){
			i = 0;
		}
		key_data = key_press_check();
		if(iJmp==JMP_ED)
		{	
			iChn=screen_get_char1(SCREEN1 , x/8 , (y+16)/8) & CFM_FONT;
			if((iChn==0)&(k==0)) /* If player is off the ground and no longer rising from a Jump, gravity is in effect */
			{
				y=y+2;
			}else{
			    if (key_data & KEY_A){
					if (k<8){
						j=3;
						k++;
						iJmp = j;
						inJump=1;
					}else{
						k=0;
					}
					if (j > JMP_ST){
						j = JMP_ST;
					}
				}else{
					j=0;
					k=0;
					if (iChn != 0){
						inJump=0;
						lMomentum=0;
						rMomentum=0;
						lunge=1;
					}
				}
			}
		}else{		
			iJmp--;		
			if (y>0){
				y=y-2;
			}
		}
		if (KEY_Y1 || KEY_Y2 || KEY_Y3 || KEY_Y4){
			yPress=1;
		}

		if (key_data & KEY_LEFT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
			}
			x--;
			if (key_data & KEY_B){
				rMomentum=0;
				if (((iChn != 0) || (lMomentum==1))){
					x = x - 2;
					lMomentum=1;
				}
			}else{
				if ((iChn == 0) & (lMomentum==1)){
					x = x - 2;
				}
			}
		}
		if (key_data & KEY_RIGHT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4color);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
			}
			x++;
			if (key_data & KEY_B){
				lMomentum=0;
				if ((iChn != 0) || (rMomentum==1)){
					x = x + 2;
					rMomentum=1;
				}
			}else{
				if ((iChn == 0) & (rMomentum==1)){
					x = x + 2;
				}
			}
		}
		/* High-Frequency Sword attacks */
		if (key_data & KEY_B){
			if ((lunge == 1) & (yPress == 1)) { 
				lunge=0;
				if (key_data & KEY_Y1) {
					y = y - 27;
				}
				else if (key_data & KEY_Y2) {
					font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
					x = x + 25;
					if (iChn==0) {
						y = y - 2;
					}
				}
				else if (key_data & KEY_Y3) {
					y = y + 27;
				}
				else if (key_data & KEY_Y4) {
					font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
					x = x - 25;
					if (iChn==0) {
						y = y - 2;
					}
				}
			}
		}
		yPress=0;
		x2 = x + 8;
		y2 = y + 8;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);
	} while((key_data & KEY_START) == 0);
	return;
}
