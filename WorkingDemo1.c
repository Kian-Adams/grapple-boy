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
	int i, iJmp, iChn;
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
	font_set_colordata(5, Ground_width * Ground_height, bmp_clear);
	screen_fill_char(SCREEN1, 0, 0, 32, 32, 0);
	screen_set_scroll(SCREEN1, 0, 0);
	groundinit();

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
	iJmp=0;	
	iChn=0;
	i=0;
	do {
		BYTE x2, y2;

		sys_wait(1);
		i++;
		if (i == 100){
			i = 0;
		}
		key_data = key_press_check();
		if(iJmp==JMP_ED)
		{	
			iChn=screen_get_char1(SCREEN1 , x/8 , (y+16)/8) & CFM_FONT;
			if(iChn==0)
			{
				y=y+2;
			}else{
			    if (key_data & KEY_A){
					iJmp=JMP_ST;
				}
			}
		}else{		
			iJmp--;		
			y=y-2;  
		}
		if (key_data & KEY_LEFT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
			}
			x--;
		}
		if (key_data & KEY_RIGHT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4color);
			}
			x++;
		}
		x2 = x + 8;
		y2 = y + 8;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);
	} while((key_data & KEY_START) == 0);
	return;
}
