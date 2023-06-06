#include <stdio.h>
#include <stdlib.h>
#include <sys/bios.h>
#include <sys/libwwc.h>
#include "Ground.h"
#include "Grapple_Boy256.h"
#include "Grapple_Boy4color-Walk.h"
#include "Grapple_Boy4color-L.h"
#include "Grapple_Boy4color-WalkL.h"
#include "GBuplunge.h"
#include "GBrightlunge.h"
#include "GBdownlunge.h"
#include "GBleftlunge.h"
#include "GBslash1.h"
#include "GBslash2.h"
#include "GBslash3.h"
#define JMP_ED 0
#define FONT_FREE 8
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
void mapload(int iX ,int iY ,char *pStr){
/*Map loading attempt, use commented out version below for current map loading version*/
    while(iX <= 27)
    {
        if( *pStr ==NULL)
            break;

        if( *pStr =='0')
        {
        }else{
            screen_fill_char(SCREEN1, iX, iY, 1, 1,2); 
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
void gameover(){
    clear_screen(SCREEN2, FALSE);
    draw_string(SCREEN2, 11, 7, FONT_FREE, "Game Over");
    display_control(DCM_SCR1 | DCM_SCR2 | DCM_SCR2_WIN_INSIDE | DCM_SPR);
}
void healthbar(int health){
	wwc_palette_set_color(24, 1, 0x0666);
	font_set_colordata(24, 1, bmp_heart);
	if (health >= 0){
		screen_fill_char(SCREEN1, 1, 0, 1, 1, 24);
    }else{
		screen_fill_char(SCREEN1, 1, 0, 1, 1, 0);
	}if (health >= 2){
		screen_fill_char(SCREEN1, 2, 0, 1, 1, 24);
	}else{
		screen_fill_char(SCREEN1, 2, 0, 1, 1, 0);
	}if (health >= 4){
		screen_fill_char(SCREEN1, 3, 0, 1, 1, 24);
	}else{
		screen_fill_char(SCREEN1, 3, 0, 1, 1, 0);
	}if (health >= 6){
		screen_fill_char(SCREEN1, 4, 0, 1, 1, 24);
	}else{
		screen_fill_char(SCREEN1, 4, 0, 1, 1, 0);
	}
}
void main(int argc, char *argv[]) {
	int i, j, k, iJmp, iChn, iChn1, iChn2, iChn3, screenscrolling, level, Phealth;
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
	screenscrolling=0;
	iJmp=0;	
	iChn=0;
    iChn1=0;
	iChn2=0;
	i=0;
	j=0;
	k=0;
	level=1;
	do {
		BYTE x2, y2;

		sys_wait(0);
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
					if (k<16){
						j=6;
						j = (j/2);
						k = k+2;
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
			if((iChn3==0)&&(x>0)){
				if (key_data & KEY_B){
					x = x - 5;
				}else{
				x=x-2;
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
					x = x + 5;
				}else{
				x=x+2;
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
		if (x > 219){
			x=0;
			screenscrolling++;
			clear_screen(SCREEN1, TRUE);
		}
		/*Calling Map Loading Function*/
		if (level == 1){
		if (screenscrolling == 0){
		mapload(0, 0, "00000000000000000000000000000000");
		mapload(0, 1, "00000000000000000000000000000000");
		mapload(0, 2, "00000000000000000000000000000000");
		mapload(0, 3, "00000000000000000000000000000000");
		mapload(0, 4, "00000000000000000000000000000000");
		mapload(0, 5, "00000000000000000000000000000000");
		mapload(0, 6, "00000000000000000000000000000000");
		mapload(0, 7, "00000000000000000000000000000000");
		mapload(0, 8, "00000000000000000000000000000000");
		mapload(0, 9, "00000000000000000000000000000000");
		mapload(0, 10, "22222222222222222222222222222222");
		mapload(0, 11, "22222222222222222222222222222222");
		mapload(0, 12, "22222222222222222222222222222222");
		mapload(0, 13, "22222222222222222222222222222222");
		mapload(0, 14, "22222222222222222222222222222222");
		mapload(0, 15, "22222222222222222222222222222222");
		mapload(0, 16, "22222222222222222222222222222222");
		mapload(0, 17, "22222222222222222222222222222222");
		}else if (screenscrolling == 1){
		mapload(0, 0, "00000000000000000000000000000000");
		mapload(0, 1, "00000000000000000000000000000000");
		mapload(0, 2, "00000000000000000000000000000000");
		mapload(0, 3, "00000000000000000000000000000000");
		mapload(0, 4, "00000000000000000000000000000000");
		mapload(0, 5, "00000000000000000000000000000000");
		mapload(0, 6, "00000000000000000000000000000000");
		mapload(0, 7, "00000000000000000000000000000000");
		mapload(0, 8, "00000000000000000000000000000000");
		mapload(0, 9, "00000000000000000000000000000000");
		mapload(0, 10, "22000000000000000000000000000000");
		mapload(0, 11, "22200000000000000000000000000000");
		mapload(0, 12, "22222222222222222222222222222222");
		mapload(0, 13, "22222222222222222222222222222222");
		mapload(0, 14, "22222222222222222222222222222222");
		mapload(0, 15, "22222222222222222222222222222222");
		mapload(0, 16, "22222222222222222222222222222222");
		mapload(0, 17, "22222222222222222222222222222222");
		}else if (screenscrolling == 2){
		mapload(0, 0, "00000000000000000000000000000000");
		mapload(0, 1, "00000000000000000000000000000000");
		mapload(0, 2, "00000000000000000000000000000000");
		mapload(0, 3, "00000000000000000000000000000000");
		mapload(0, 4, "00000000000000000000000000000000");
		mapload(0, 5, "00000000000000000000000000000000");
		mapload(0, 6, "00000000000000000000000000000000");
		mapload(0, 7, "00000000000000000000000000000000");
		mapload(0, 8, "00000000000000000000000000000000");
		mapload(0, 9, "00000000000000000000000000000000");
		mapload(0, 10, "00000000000000000000000000000000");
		mapload(0, 11, "00000000000000000000000000000000");
		mapload(0, 12, "22222222222200000002222222222222");
		mapload(0, 13, "22222222222200000002222222222222");
		mapload(0, 14, "22222222222200000002222222222222");
		mapload(0, 15, "22222222222200000002222222222222");
		mapload(0, 16, "22222222222200000002222222222222");
		mapload(0, 17, "22222222222200000002222222222222");
		}else if (screenscrolling == 3){
		mapload(0, 0, "20000000000000000000000000000000");
		mapload(0, 1, "20000000000000000000000000000000");
		mapload(0, 2, "20000222222222222222222222220000");
		mapload(0, 3, "20000000000020000000000000000000");
		mapload(0, 4, "20000000000020000000000000000000");
		mapload(0, 5, "20000000000020000000000000000000");
		mapload(0, 6, "20000000000020000000000000000000");
		mapload(0, 7, "20000000000020000000000000000000");
		mapload(0, 8, "20000000000020000000000000000000");
		mapload(0, 9, "00000000000020000000000000000000");
		mapload(0, 10, "00000000000020000000000000000000");
		mapload(0, 11, "00000000000020000000000000000000");
		mapload(0, 12, "22222222222200000000000000000222");
		mapload(0, 13, "22222222222200000000000000000222");
		mapload(0, 14, "22222222222200000000000000000222");
		mapload(0, 15, "22222222222200000000000000000222");
		mapload(0, 16, "22222222222200000000000000000222");
		mapload(0, 17, "22222222222200000000000000000222");
		}else if (screenscrolling == 4){
		mapload(0, 0, "00002000000000000000000000200000");
		mapload(0, 1, "00002000000000000000000000200000");
		mapload(0, 2, "00002000000000000000000000200000");
		mapload(0, 3, "00002000000000000000000000200000");
		mapload(0, 4, "00002000000000000000000000200000");
		mapload(0, 5, "00002000000000000000000000200000");
		mapload(0, 6, "00002000000000000000000000200000");
		mapload(0, 7, "00002000000000000000000000200000");
		mapload(0, 8, "00002000000000000000000000200000");
		mapload(0, 9, "00002000000000000000000000200000");
		mapload(0, 10, "22222222222222222222222222000000");
		mapload(0, 11, "22222222222222222222222222222222");
		mapload(0, 12, "22222222222222222222222222222222");
		mapload(0, 13, "22222222222222222222222222222222");
		mapload(0, 14, "22222222222222222222222222222222");
		mapload(0, 15, "22222222222222222222222222222222");
		mapload(0, 16, "22222222222222222222222222222222");
		mapload(0, 17, "22222222222222222222222222222222");
		}else if (screenscrolling == 5){
		mapload(0, 0, "22222222222222222222222222222222");
		mapload(0, 1, "22222222222222222222222222222222");
		mapload(0, 2, "22222222222222222222222222222222");
		mapload(0, 3, "22222222222222222222222222222222");
		mapload(0, 4, "00000000000000000000000000000000");
		mapload(0, 5, "00000000000000000000000000000000");
		mapload(0, 6, "00000000000000000000000000000000");
		mapload(0, 7, "00000000000000000000000000000000");
		mapload(0, 8, "00000000000000000000000000000000");
		mapload(0, 9, "00000000000000000000000000000000");
		mapload(0, 10, "00000000000000000000000000000000");
		mapload(0, 11, "00000000000000000000000000000000");
		mapload(0, 12, "22200000222000000002220000022222");
		mapload(0, 13, "22200000222000000002220000022222");
		mapload(0, 14, "22200000222000000002220000022222");
		mapload(0, 15, "22200000222000000002220000022222");
		mapload(0, 16, "22200000222000000002220000022222");
		mapload(0, 17, "22200000222000000002220000022222");
		}else if (screenscrolling == 6){
		mapload(0, 0, "22222222222222222222222222222222");
		mapload(0, 1, "22222222222222222222222222222222");
		mapload(0, 2, "22222222222222222222222222222222");
		mapload(0, 3, "22222222222222222222222222222222");
		mapload(0, 4, "00000000000000020000000000000000");
		mapload(0, 5, "00000000000000020000000000000000");
		mapload(0, 6, "00000000000000020000000000000000");
		mapload(0, 7, "00000000000000020000000000000000");
		mapload(0, 8, "00000000000000020000000000000000");
		mapload(0, 9, "00000000000000020000000000000000");
		mapload(0, 10, "00000000000000020000000000000000");
		mapload(0, 11, "00000000000000020000000000000000");
		mapload(0, 12, "22200000000222222220000000022222");
		mapload(0, 13, "22200000000222222220000000022222");
		mapload(0, 14, "22200000000222222220000000022222");
		mapload(0, 15, "22200000000222222220000000022222");
		mapload(0, 16, "22200000000222222220000000022222");
		mapload(0, 17, "22200000000222222220000000022222");
		}else if (screenscrolling == 7){
			level++;
		}
		}else if(level == 2){

		}else if (level == 3){

		}
		if (y > 200){
			Phealth = 0;
		}
		if (Phealth == 0){
			gameover();
			sys_wait(100);
			key_data = 'end';
			break;
		}else{
			healthbar(Phealth);
		}
	} while((key_data & KEY_START) == 0);
	return;
}