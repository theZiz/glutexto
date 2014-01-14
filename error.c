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
 
char error_message[1024];

void draw_error()
{
	int height = 2;
	int i;
	for (i = 0; error_message[i]!=0; i++)
		if (error_message[i] == '\n')
			height++;
	spRectangle(screen->w/2,screen->h/2,0,screen->w/2,(height)*font->maxheight,BACKGROUND_COLOR);
	/*int pos = screen->h/4-(4+menu_count)*font->maxheight/2;
	spFontDrawMiddle(screen->w*ERROR_WIDTH,pos,0,menu_name,font);
	pos+=font->maxheight*2;
	int i;
	pMenuItem item = firstMenuItem;
	for (i = 0;i < menu_count; i++)
	{
		if ( i == menu_sel )
		{
			char buffer[64];
			sprintf(buffer,"> %s <",item->name);
			spFontDrawMiddle(screen->w*ERROR_WIDTH,pos,0,buffer,font);
			int l = spFontWidth(buffer,font);
			spLine(screen->w*ERROR_WIDTH-l/2,pos+font->maxheight-1,0,screen->w*ERROR_WIDTH+l/2,pos+font->maxheight-1,0,FONT_COLOR);
		}
		else
			spFontDrawMiddle(screen->w*ERROR_WIDTH,pos,0,item->name,font);
		pos+=font->maxheight;
		item = item->next;
	}
	spFontDrawMiddle(screen->w*ERROR_WIDTH,screen->h/4+(2+menu_count)*font->maxheight/2,0,SP_PRACTICE_OK_NAME": Choose    "SP_PRACTICE_CANCEL_NAME": Back",font);*/
	spFlip();
}

int calc_error(Uint32 steps)
{
	if (spGetInput()->button[SP_PRACTICE_OK])
	{
		spGetInput()->button[SP_PRACTICE_OK] = 0;
		return 1;
	}
	if (spGetInput()->button[SP_PRACTICE_CANCEL])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL] = 0;
		return -1;
	}
	return 0;
}

int ask_yes_no(char* message)
{
	sprintf(error_message,"%s",message);
	return (spLoop( draw_error, calc_error, 10, resize, NULL ) == 1);
}
