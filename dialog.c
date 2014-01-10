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

int dialog_kind = 0;
char dialog_filename[512];

void draw_dialog()
{
	spClearTarget(BACKGROUND_COLOR);
	switch (dialog_kind)
	{
		case 0:
			spFontDrawMiddle(screen->w/2,0,0,"Choose file to load",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose file or enter folder    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
		case 1:
			spFontDrawMiddle(screen->w/2,0,0,"Choose location to save",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose or enter folder    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
		case 2:
			spFontDrawMiddle(screen->w/2,0,0,"Choose filename to save",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose letter    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
	}
	
	spFlip();
}

int calc_dialog(Uint32 steps)
{
	if (spGetInput()->button[SP_PRACTICE_CANCEL])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL] = 1;
		return 1;
	}
	/*if (spGetInput()->button[SP_PRACTICE_OK])
	{
		spGetInput()->button[SP_PRACTICE_OK] = 1;
		return 1;
	}*/
	return 0;
}

void run_dialog(int kind)
{
	dialog_filename[0] = 0;
	dialog_kind = kind;
	spLoop(draw_dialog,calc_dialog,10,resize,NULL);
}

void load_dialog()
{
	run_dialog(0);
}

void save_as_dialog()
{
	run_dialog(1);
}
