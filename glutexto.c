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
#define BACKGROUND_COLOR spGetRGB(96,32,0)
#define EDIT_BACKGROUND_COLOR spGetRGB(255,255,255)
#define EDIT_NUMBER_BACKGROUND_COLOR spGetRGB(200,200,200)
#define EDIT_TEXT_COLOR spGetRGB(32,0,0)
#define EDIT_LINE_COLOR spGetRGB(220,220,220)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

typedef struct sFont *pFont;
typedef struct sFont {
	char name[256];
	char location[256];
	pFont next;
} tFont;

typedef struct sText *pText;
typedef struct sText {
	char* line;
	int length;
	int reserved;
	pText prev,next;
} tText;

pFont firstFont = NULL;
pFont selectedFont = NULL;
#define MIN_FONT_SIZE 6
#define MAX_FONT_SIZE 20
int fontSize = 11;
int exit_now = 0;
SDL_Surface* screen;
SDL_Surface* editSurface = NULL;
spFontPointer font = NULL;
spFontPointer fontInverted = NULL;
spFontPointer textFont = NULL;
pText text = NULL;
pText momLine = NULL;
int line_number = 0;
int line_pos = 0;
int showLines = 1;
char dialog_folder[512] = "/usr/local/home";
int next_in_a_row = 0;
int time_until_next = 0;
int blink = 0;
char enter_buffer[5]="a"; //a whole utf8 letter

void resize(Uint16 w,Uint16 h);
void draw_without_flip();

#include "error.c"
#include "text.c"
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
	int max_line = 1;
	int count = line_count;
	while (count > 0)
	{
		count = count/10;
		max_line++;
	}
	char buffer[256];
	int number_width;
	if (showLines)
	{
		for (i = 0; i < max_line; i++)
			buffer[i]='8';
		buffer[i]=0;
		number_width = spFontWidth(buffer,textFont);
	}
	int lines_per_screen = editSurface->h/(textFont->maxheight+extra);
	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		lines_per_screen = (editSurface->h-spGetVirtualKeyboard()->h)/(textFont->maxheight+extra);
	
	int start_line = line_number - lines_per_screen/3;
	if (start_line+lines_per_screen+1 > line_count)
		start_line = line_count-lines_per_screen+1;
	if (start_line < 1)
		start_line = 1;
	pText line = momLine;
	for (i = line_number; i > start_line; i--)
		line = line->prev;
	int number = start_line;

	char end = momLine->line[line_pos];
	momLine->line[line_pos] = 0;
	int momLineCursorPos = spFontWidth(momLine->line,textFont);
	momLine->line[line_pos] = end;
	
	int textShift = 1;

	if (momLineCursorPos > editSurface->w*3/4)
	{
		textShift = editSurface->w*3/4 - momLineCursorPos;
	}

	for (i = extra; i < editSurface->h && line; i+=textFont->maxheight+extra)
	{
		int text_extra = 0;
		if (showLines)
		{
			spFontDraw(textShift+number_width,i,0,line->line,textFont);
			spSetPattern8(pattern,pattern,pattern,pattern,pattern,pattern,pattern,pattern);
			spLine(number_width,i+letter->height,number_width,screen->w,i+letter->height,0,EDIT_LINE_COLOR);
			spDeactivatePattern();
			sprintf(buffer,"%i:",number);
			spRectangle(number_width/2-2,i+(textFont->maxheight+extra-2)/2,0,number_width,textFont->maxheight+extra,EDIT_NUMBER_BACKGROUND_COLOR);
			spFontDrawRight(number_width-1,i,0,buffer,textFont);
			text_extra = number_width-1;
		}
		else
		{
			spSetPattern8(pattern,pattern,pattern,pattern,pattern,pattern,pattern,pattern);
			spLine(0,i+letter->height,0,screen->w,i+letter->height,0,EDIT_LINE_COLOR);
			spDeactivatePattern();
			spFontDraw(textShift,i,0,line->line,textFont);
		}
		if (line == momLine)
		{
			text_extra += momLineCursorPos;
			if (!(blink & 512))
			{
				spLine(textShift+text_extra-1,i+letter->height-font->maxheight-extra,0,textShift+text_extra+2,i+letter->height-font->maxheight-extra,0,EDIT_TEXT_COLOR);
				spLine(textShift+text_extra-1,i+letter->height,0,textShift+text_extra+2,i+letter->height,0,EDIT_TEXT_COLOR);
				spLine(textShift+text_extra,i+letter->height-font->maxheight-extra,0,textShift+text_extra,i+letter->height,0,EDIT_TEXT_COLOR);
			}
		}
		number++;
		line = line->next;
	}
	//drawing all
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spBlitSurface( screen->w/2,screen->h/2,0,editSurface);
	if (text_changed)
		sprintf(buffer,"*%s",last_filename);
	else
		sprintf(buffer,"%s",last_filename);
	spFontDraw(0,0,0,buffer,font);

	if (spIsKeyboardPolled() && spGetVirtualKeyboardState() == SP_VIRTUAL_KEYBOARD_ALWAYS)
		spBlitSurface(screen->w/2,screen->h-spGetVirtualKeyboard()->h/2-font->maxheight,0,spGetVirtualKeyboard());

	if (!spIsKeyboardPolled())
	{
		spFontDrawMiddle(screen->w/2,0,0,SP_BUTTON_START_NAME": Main",font);
		spFontDrawRight(screen->w,0,0,SP_BUTTON_SELECT_NAME": Options",font);
		spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,\
			SP_PRACTICE_OK_NAME": Enter text   "\
			SP_PRACTICE_3_NAME": Load   "\
			SP_PRACTICE_4_NAME": Save",font);
	}
	else
		spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,\
			SP_PRACTICE_OK_NAME": Enter letter   "\
			SP_PRACTICE_CANCEL_NAME": Finish   "\
			SP_PRACTICE_3_NAME": Return   "\
			SP_PRACTICE_4_NAME": Backspace",font);
}

void draw()
{
	draw_without_flip();
	spFlip();
}

int calc(Uint32 steps)
{
	blink += steps;
	if (!spIsKeyboardPolled())
	{
		if (time_until_next > 0)
			time_until_next -= steps;
		if (spGetInput()->axis[1] < 0 && momLine->prev)
		{
			if (time_until_next <= 0)
			{
				momLine = momLine->prev;
				line_number--;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
				if (line_pos > momLine->length)
					line_pos = momLine->length;
			}
		}
		else
		if (spGetInput()->axis[1] > 0 && momLine->next)
		{
			if (time_until_next <= 0)
			{
				momLine = momLine->next;
				line_number++;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
				if (line_pos > momLine->length)
					line_pos = momLine->length;
			}
		}
		else
		if (spGetInput()->button[SP_BUTTON_L] && momLine->prev)
		{
			if (time_until_next <= 0)
			{
				int i;
				for (i = 0; i < 32 && momLine->prev; i++)
					momLine = momLine->prev;
				line_number-=i;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
				if (line_pos > momLine->length)
					line_pos = momLine->length;
			}
		}
		else
		if (spGetInput()->button[SP_BUTTON_R] && momLine->next)
		{
			if (time_until_next <= 0)
			{
				int i;
				for (i = 0; i < 32 && momLine->next; i++)
					momLine = momLine->next;
				line_number+=i;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
				if (line_pos > momLine->length)
					line_pos = momLine->length;
			}
		}
		else
		if (spGetInput()->axis[0] < 0 && line_pos > 0)
		{
			if (time_until_next <= 0)
			{
				line_pos--;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
			}
		}
		else
		if (spGetInput()->axis[0] > 0 && line_pos < momLine->length)
		{
			if (time_until_next <= 0)
			{
				line_pos++;
				next_in_a_row++;
				time_until_next = 300/next_in_a_row;
				blink = 0;
			}
		}
		else
		{
			time_until_next = 0;
			next_in_a_row = 0;
		}
	}
	if (!spIsKeyboardPolled())
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
		if (spGetInput()->button[SP_PRACTICE_4])
		{
			spGetInput()->button[SP_PRACTICE_4] = 0;
			menu_save(0,NULL);
		}
		if (spGetInput()->button[SP_PRACTICE_OK])
		{
			spGetInput()->button[SP_PRACTICE_OK] = 0;
			spPollKeyboardInput(enter_buffer,4,SP_PRACTICE_OK_MASK);
		}
	}
	else
	{
		if (spGetInput()->button[SP_PRACTICE_CANCEL])
		{
			spGetInput()->button[SP_PRACTICE_CANCEL] = 0;
			spStopKeyboardInput();
		}
		if (spGetInput()->button[SP_PRACTICE_3])
		{
			spGetInput()->button[SP_PRACTICE_3] = 0;
			addReturn();
		}
		if (spGetInput()->button[SP_PRACTICE_4])
		{
			spGetInput()->button[SP_PRACTICE_4] = 0;
			enter_buffer[0] = 0;
		}
	}
	if (enter_buffer[1] != 0)
	{
		addToLine(&enter_buffer[1]);
		enter_buffer[0] = 'a';
		enter_buffer[1] = 0;
		spGetInput()->keyboard.pos = 1;
	}
	if (enter_buffer[0] == 0)
	{
		removeFromLine();
		enter_buffer[0] = 'a';
		enter_buffer[1] = 0;
		spGetInput()->keyboard.pos = 1;
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
	spFontAdd(font,SP_FONT_GROUP_ASCII""SP_FONT_GROUP_GERMAN,FONT_COLOR);//whole ASCII
	spFontAddBorder(font,BACKGROUND_COLOR);
	spFontMulWidth(font,14<<SP_ACCURACY-4);

	spFontSetShadeColor(EDIT_BACKGROUND_COLOR);
	if (fontInverted)
		spFontDelete(fontInverted);
	fontInverted = spFontLoad(FONT_LOCATION,FONT_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(fontInverted,SP_FONT_GROUP_ASCII""SP_FONT_GROUP_GERMAN,EDIT_TEXT_COLOR);//whole ASCII
	spFontAddBorder(fontInverted,EDIT_BACKGROUND_COLOR);
	spFontMulWidth(fontInverted,14<<SP_ACCURACY-4);

	if (editSurface)
		spDeleteSurface(editSurface);
	editSurface = spCreateSurface(w,h-2*font->maxheight);

	spSetVirtualKeyboard(SP_VIRTUAL_KEYBOARD_ALWAYS,0,h-w*48/320-font->maxheight,w,w*48/320,spLoadSurface("./data/keyboard320.png"),spLoadSurface("./data/keyboardShift320.png"));
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
	newText();
}

void quit_glutexto()
{
	clearText();
	spFontDelete(font);
	spFontDelete(fontInverted);
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
