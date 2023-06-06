#include <stdio.h>
#include <stdlib.h>
#include <sys/bios.h>
#include <sys/libwwc.h>
#include "Ground.h"
#include "Grapple_Boy256.h"
#include "Grapple_Boy4color-Walk.h"
#include "Grapple_Boy4color-L.h"
#include "Grapple_Boy4color-WalkL.h"
#define JMP_ED 0
#define ANITIME    10 
int fncLoop(int iArg ,int iPitch)
{
    if(iArg < 0)
        iArg = iArg + iPitch;

    if(iArg >= iPitch)
        iArg = iArg - iPitch;

    return(iArg);
}

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
/*All 3 functions below make a basic platform, not used in this version of the build.*/
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
    subPlatformSet(SCREEN1 ,0 ,7,27, 12                  ,0,2);
    subTextureSet(SCREEN2 ,0 ,7,27, 12                  ,0,2);
	subPlatformSet(SCREEN1 ,0 ,7,14, 12                  ,0,2);
    subTextureSet(SCREEN2 ,0 ,7,14, 12                  ,0,2);
}
void mapload(int iX ,int iY ,char *pStr){
/*Map loading attempt, use commented out version below for current map loading version*/
    for(;;)
    {
        if( *pStr ==NULL)
            break;

        if( *pStr ==' ')
        {
            screen_fill_char(SCREEN2, iX, iY, 1, 1 ,CFM_BGR | CHN_TOMEBOX); 
        }else{
            screen_fill_char(SCREEN1, iX, iY, 1, 1 ,CFM_MOJI | ((*pStr-'1')+CHN_KUROBOX1)); 
        }
        iX++;
        pStr++;
    }
	/*
	static int iS2x=0;     
    static int iS1x=0;       
    static int iSfx=(224/8);    
    static int iMap=0;      
    static int iCnt=0;
	int z = 0;
	char ch;
    iCnt=fncLoop(iCnt,10); 
    if(iCnt==0)
    {
        iS2x=fncLoop(iS2x,SCREEN_PIXEL_WIDTH);
            if(z<MEN_H){

                ch = cMap[iMap][(MEN_H-1)-z];  
				
                if(ch != '2') {
                    /*subBone(pEi ,224 ,((MEN_H-1)-z)*8 ,ch); 
                    screen_fill_char(SCREEN2 ,iSfx ,z ,1, 1 ,2);
                }else{
				screen_fill_char(SCREEN1 ,iSfx ,z ,1, 1 ,2);
				z++;
            }
            }
			if ((z==MEN_H)&(iSfx<=MEN_W)){
				z=0;
				iSfx++;
			}
            iMap=fncLoop(iMap+1,MEN_W); 

            iS1x=0;
        }*/
}
/*Main*/
void main(int argc, char *argv[]) {
	int i, j, k, iJmp, iChn, iChn1, iChn2, iChn3;
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
	groundinit();
	clear_screen(SCREEN1, TRUE);
	font_set_colordata(0, Ground_width * Ground_height, bmp_clear);
	font_set_colordata(2, Ground_width * Ground_height, bmp_Ground);
	/* screen_fill_char(SCREEN2, 0, 0, 32, 32, 0);



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
    iChn1=0;
	iChn2=0;
	i=0;
	j=0;
	k=0;
	do {
		BYTE x2, y2;

		sys_wait(1);
		i++;
		if (i == 3){
			i = 0;
		}
		key_data = key_press_check();
		if(iJmp==JMP_ED)
		{	/*All 4 collision areas, iChn2 and 3 are used for side to side collision.*/
			iChn=screen_get_char1(SCREEN1 , (x2/8)+1, (y+16)/8) & CFM_FONT;
			iChn3=screen_get_char1(SCREEN1 , (x2-8)/8, (y+15)/8) & CFM_FONT;
            iChn1=screen_get_char1(SCREEN1 , (x)/8, (y+16)/8) & CFM_FONT;
            iChn2=screen_get_char1(SCREEN1 , (x+16)/8, (y+15)/8) & CFM_FONT;
			if((iChn==0)&&(iChn1==0)&(k==0))
			{
				y=y+2;
			}else{
			    if (key_data & KEY_A){
					if (k<8){
						j=6;
						j = (j/2);
						k++;
						iJmp = j;
					}else{
						k=0;
					}
					if (j > k){
						j = k;
					}
				}else{
					j=0;
					k=0;
				}
			}
		}else{		
			iJmp--;		
			if (y>0){
			y=y-2;
			}  
		}
		if (key_data & KEY_LEFT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
			}
			if((iChn3==0)){
				if (key_data & KEY_B){
					x = x - 3;
				}else{
				x--;
			}
			}
		}
		if (key_data & KEY_RIGHT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4color);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalk);
			}
			if((iChn2==0)){
				if (key_data & KEY_B){
					x = x + 3;
				}else{
				x++;
			}
			}
		}
		if (key_data & KEY_DOWN2){
			y = y + 8;
		}
		x2 = x + 8;
		y2 = y + 8;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);
		/*Looping character around to start of the screen for possibility of Mega Man Style map switching.*/
		if (x > 220){
			x=0;
		}
		/*Calling Map Loading Function*/
		mapload();
	} while((key_data & KEY_START) == 0);
	return;
}