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

#define MEN_W  48
#define MEN_H  18
static unsigned char cMap[MEN_W+1][MEN_H+1]={
        {"220000000000000000"},
        {"220000000000000000"},
        {"222000000000000002"},
        {"22220000B000000002"},
        {"222220000000000222"},
        {"222220000000000022"},
        {"222220000000000022"},
        {"222220000000000022"},
        {"222220000000000000"},
        {"222200000000000000"},
        {"222000000000000000"},
        {"2200000B0000000222"},
        {"220000000000000222"},
        {"220000000000000222"},
        {"220000000000000022"},
        {"220000000000000002"},
        {"222000000000000002"},
        {"222000000000000002"},
        {"222000000000000002"},
        {"222200000000000002"},
        {"222000000000000002"},
        {"222000000000000002"},
        {"220000000000000022"},
        {"222222220000000022"},
        {"222222222000000022"},
		/*
        {"210000000000000022"},
        {"2100000B0000000000"},
        {"230000000000000000"},
        {"223000000C00000000"},
        {"2223A0000000A00000"},
        {"222400000000000000"},
        {"224000000000000000"},
        {"240000000000000002"},
        {"200000000000000002"},
        {"20022230B000000002"},
        {"222222100000000222"},
        {"2222221000B0000222"},
        {"222222400000000222"},
        {"224000000C00000022"},
        {"22220000C000000022"},
        {"22210000000C000022"},
        {"222200C00000022222"},
        {"220100000000022222"},
        {"222300000000B00000"},
        {"222230000000000000"},
        {"222210B000B0000000"},
        {"222210000000000022"},
        {"222240000000000022"},
		*/
};

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
/*--------------------------------画面スクロール*/
void main(int argc, char *argv[]) {
	static int iS2x=0;        /*スクリーン2スクロール座標*/
    static int iS1x=0;        /*スクリーン1スクロール座標*/
    static int iSfx=(224/8);     /*スクリーン描画位置*/
    static int iMap=0;        /*仮想マップ参照位置*/
    static int iCnt=0;
	int i, j, k, z, iJmp, iChn, iChn2;
	char ch;
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
		{	
			iChn=screen_get_char1(SCREEN1 , x/8 , (y+16)/8) & CFM_FONT;
			if((iChn==0)&(k==0))
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
		iChn2=0;
		/*screen_get_char1(SCREEN1 , (x+16)/8 , (x+16)/8) & CFM_FONT;*/
		if (key_data & KEY_LEFT1){
			if ((i%10)==0){
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorL);
			}else{
				font_set_colordata(4, 16, bmp_Grapple_Boy4colorWalkL);
			}
			if(iChn2==0){
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
			if(iChn2==0){
				if (key_data & KEY_B){
					x = x + 3;
				}else{
				x++;
			}
			}
		}
		if (key_data & KEY_RIGHT2){
			y = y + 8;
		}
		x2 = x + 8;
		y2 = y + 8;
		sprite_set_location(0, x, y);
		sprite_set_location(1, x2, y);
		sprite_set_location(2, x, y2);
		sprite_set_location(3, x2, y2);
		/* WHEN IT IS TIME FOR THE LUNGE ATTACKS, USE X2, X3, ETC. ADD IN THE NEW SPRITE SET LOCATIONS IN AN IF LOOP.*/
    iCnt=fncLoop(iCnt-1,ANITIME);
    if(iCnt==0)
    {
        iS2x=fncLoop(iS2x + 1 ,SCREEN_PIXEL_WIDTH);

        if((iS2x % 8)==1) {
            for(z=0 ;z<MEN_H ;z++) {

                ch = cMap[iMap][(MEN_H-1)-z];  
				
                if(ch != '2') {
                    /*subBone(pEi ,224 ,((MEN_H-1)-z)*8 ,ch); */
                    ch = '0';
                }
                ch -= '0';
                ch += 0;
				if (ch == 0){
					screen_fill_char(SCREEN2 ,iSfx ,z ,1, 1 ,ch);
				}else{
					screen_fill_char(SCREEN1 ,iSfx ,z ,1, 1 ,ch);
				}
            }
            iSfx=fncLoop(iSfx + 1 ,SCREEN_CHAR_WIDTH);
            iMap=fncLoop(iMap+1,MEN_W); 

            iS1x=fncLoop(iS1x + 1 ,SCREEN_PIXEL_WIDTH);
        }
    }
	} while((key_data & KEY_START) == 0);
	return;
}
