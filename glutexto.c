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
#define BACKGROUND_COLOR spGetRGB(0,64,0)
#define EDIT_BACKGROUND_COLOR spGetRGB(255,255,255)
#define EDIT_NUMBER_BACKGROUND_COLOR spGetRGB(200,200,200)
#define EDIT_TEXT_COLOR spGetRGB(0,32,0)
#define EDIT_LINE_COLOR spGetRGB(220,220,220)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

typedef struct sFont *pFont;
typedef struct sFont {
	char name[256];
	char location[256];
	pFont next;
} tFont;

pFont firstFont = NULL;
pFont selectedFont = NULL;
#define MIN_FONT_SIZE 6
#define MAX_FONT_SIZE 20
int fontSize = 12;
int exit_now = 0;
SDL_Surface* screen;
SDL_Surface* editSurface = NULL;
spFontPointer font = NULL;
spFontPointer textFont = NULL;
int showLines = 1;


void resize(Uint16 w,Uint16 h);
void draw_without_flip();

#include "settings.c"
#include "dialog.c"
#include "menu.c"

void load_fonts()
{
	spFileListPointer result;
	spFileGetDirectory(&result,"./font",0,0);
	spFileListPointer directory = result;
	while (directory)
	{
		if (directory->type == SP_FILE_DIRECTORY)
		{
			spFileListPointer subresult;
			spFileGetDirectory(&subresult,directory->name,0,0);
			spFileListPointer file = subresult;
			while (file)
			{
				if (strcmp(&file->name[strlen(file->name)-4],".ttf") == 0)
				{
					pFont font = (pFont)malloc(sizeof(tFont));
					char without_ttf[256];
					sprintf(without_ttf,"%s",file->name);
					without_ttf[strlen(without_ttf)-4] = 0;
					if (strcmp(&without_ttf[strlen(without_ttf)-8],"-Regular") == 0)
						without_ttf[strlen(without_ttf)-8] = 0;
					sprintf(font->name,"%s",&without_ttf[strlen(directory->name)+1]);
					sprintf(font->location,"%s",file->name);
					font->next = firstFont;
					firstFont = font;
				}
				file = file->next;
			}
			spFileDeleteList(subresult);
		}
		directory = directory->next;
	}
	spFileDeleteList(result);
	selectedFont = firstFont;
}

void draw_without_flip( void )
{
	//filling the edit field
	spSelectRenderTarget(editSurface);
	spClearTarget( EDIT_BACKGROUND_COLOR );
	int i;
	int pattern = 0b11001100;
	spLetterPointer letter = spFontGetLetter(textFont,'A');
	int extra = fontSize/4;
	int max_line = 2+1; //TODO
	char buffer[256];
	int number_width;
	if (showLines)
	{
		for (i = 0; i < max_line; i++)
			buffer[i]='8';
		buffer[i]=0;
		number_width = spFontWidth(buffer,textFont);
		spRectangle(number_width/2-2,editSurface->h/2,0,number_width,editSurface->h,EDIT_NUMBER_BACKGROUND_COLOR);
	}
	int line_number = 1;
	for (i = extra; i < editSurface->h; i+=textFont->maxheight+extra)
	{
		if (showLines)
		{
			spSetPattern8(pattern,pattern,pattern,pattern,pattern,pattern,pattern,pattern);
			spLine(number_width,i+letter->height,number_width,screen->w,i+letter->height,0,EDIT_LINE_COLOR);
			spDeactivatePattern();
			sprintf(buffer,"%i:",line_number);
			spFontDrawRight(number_width-1,i,0,buffer,textFont);
			spFontDraw(number_width,i,0,"Testtext",textFont);
		}
		else
		{
			spSetPattern8(pattern,pattern,pattern,pattern,pattern,pattern,pattern,pattern);
			spLine(0,i+letter->height,0,screen->w,i+letter->height,0,EDIT_LINE_COLOR);
			spDeactivatePattern();
			spFontDraw(0,i,0,"Testtext",textFont);
		}
		line_number++;
	}
	//drawing all
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spBlitSurface( screen->w/2,screen->h/2,0,editSurface);
	spFontDrawMiddle(screen->w/2,0,0,SP_BUTTON_START_NAME": Main",font);
	spFontDrawRight(screen->w,0,0,SP_BUTTON_SELECT_NAME": Options",font);
	spFontDraw(0,0,0,"Glutexto",font);

	spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,\
		SP_PRACTICE_OK_NAME": Enter    "\
		SP_PRACTICE_CANCEL_NAME": Finish    "\
		SP_PRACTICE_3_NAME": Load    "\
		SP_PRACTICE_4_NAME": Save",font);
}

void draw()
{
	draw_without_flip();
	spFlip();
}

int calc(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
	{
		spGetInput()->button[SP_BUTTON_START] = 0;
		main_menu();
	}
	if (spGetInput()->button[SP_BUTTON_SELECT])
	{
		spGetInput()->button[SP_BUTTON_SELECT] = 0;
		options_menu();
	}
	if (spGetInput()->button[SP_PRACTICE_3])
	{
		spGetInput()->button[SP_PRACTICE_3] = 0;
		load_dialog();
	}
	return exit_now;
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
	load_fonts();
	load_settings();
	spFontSetShadeColor(EDIT_BACKGROUND_COLOR);
	if (textFont)
		spFontDelete(textFont);
	textFont = spFontLoad(selectedFont->location,fontSize*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(textFont,SP_FONT_GROUP_ASCII,EDIT_TEXT_COLOR);//whole ASCII
}

void quit_glutexto()
{
	spFontDelete(font);
	spFontDelete(textFont);
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
