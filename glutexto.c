 /* This file is part of glutexto.
  * glutexto is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * glutexto is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with glutexto.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#define GCW_FEELING

#if defined GCW_FEELING && defined X86CPU
	#define TESTING
	#define GCW
	#undef X86CPU
#endif
#include <sparrow3d.h>
#if defined GCW_FEELING && defined TESTING
	#define X86CPU
	#undef GCW
#endif

#define VERSION "0.1.0.0"
#define FONT_LOCATION "./font/Chango-Regular.ttf"
#define FONT_SIZE 7
#define FONT_COLOR spGetRGB(255,255,255)
#define BACKGROUND_COLOR spGetRGB(0,0,128)
#define EDIT_BACKGROUND_COLOR spGetRGB(255,255,255)
#define EDIT_LINE_COLOR spGetRGB(220,220,220)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

SDL_Surface* screen;
SDL_Surface* editSurface = NULL;
spFontPointer font = NULL;

void draw( void )
{
	//filling the edit field
	spSelectRenderTarget(editSurface);
	spClearTarget( EDIT_BACKGROUND_COLOR );
	int i;
	int pattern = 0b11001100;
	spSetPattern8(pattern,pattern,pattern,pattern,pattern,pattern,pattern,pattern);
	for (i = 0; i < editSurface->h; i+=font->maxheight)
		spLine(0,i,0,screen->w,i,0,EDIT_LINE_COLOR);
	spDeactivatePattern();
	//drawing all
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spBlitSurface( screen->w/2,screen->h/2,0,editSurface);
	spFontDrawMiddle(screen->w/2,0,0,SP_BUTTON_START_NAME": File Menu",font);
	spFontDrawRight(screen->w,0,0,SP_BUTTON_SELECT_NAME": Extras",font);
	spFontDraw(0,0,0,"Glutexto",font);

	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,\
		SP_PRACTICE_OK_NAME": Enter    "\
		SP_PRACTICE_CANCEL_NAME": Finish    "\
		SP_PRACTICE_3_NAME": Load    "\
		SP_PRACTICE_4_NAME": Save",font);
	spFlip();
}

int calc(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  
	spFontShadeButtons(1);
	//Font Loading
	spFontSetShadeColor(BACKGROUND_COLOR);
	if (font)
		spFontDelete(font);
	font = spFontLoad(FONT_LOCATION,FONT_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(font,SP_FONT_GROUP_ASCII,FONT_COLOR);//whole ASCII
	spFontAddBorder(font,BACKGROUND_COLOR);
	spFontMulWidth(font,15<<SP_ACCURACY-4);
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'w', SP_BUTTON_UP_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'q', SP_BUTTON_L_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'e', SP_BUTTON_R_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'o', SP_PRACTICE_OK_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'c', SP_PRACTICE_CANCEL_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

  if (editSurface)
		spDeleteSurface(editSurface);
	editSurface = spCreateSurface(w,h-2*font->maxheight);

}

void init_glutexto()
{
	spInitCore();
	spSetAffineTextureHack(0); //We don't need it :)
	spInitMath();
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	spSetZSet(0);
	spSetZTest(0);
}

void quit_glutexto()
{
	spFontDelete(font);
	spQuitCore();
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	init_glutexto();
	spLoop( draw, calc, 10, resize, NULL );
	quit_glutexto();
	return 0;
}
