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

typedef struct sMenuItem *pMenuItem;
typedef struct sMenuItem {
	char name[64];
	void ( *callBack )( void );
	pMenuItem next;
} tMenuItem;

pMenuItem firstMenuItem = NULL;

int menu_count = 0;
int menu_sel = 0;
char menu_name[64];

void draw_menu()
{
	spRectangle(screen->w*5/7,screen->h/4,0,screen->w/2,(4+menu_count)*font->maxheight,BACKGROUND_COLOR);
	int pos = screen->h/4-(4+menu_count)*font->maxheight/2;
	spFontDrawMiddle(screen->w*5/7,pos,0,menu_name,font);
	pos+=font->maxheight*2;
	int i;
	pMenuItem item = firstMenuItem;
	for (i = 0;i < menu_count; i++)
	{
		if ( i == menu_sel )
		{
			char buffer[64];
			sprintf(buffer,"> %s <",item->name);
			spFontDrawMiddle(screen->w*5/7,pos,0,buffer,font);
			int l = spFontWidth(buffer,font);
			spLine(screen->w*5/7-l/2,pos+font->maxheight-1,0,screen->w*5/7+l/2,pos+font->maxheight-1,0,FONT_COLOR);
		}
		else
			spFontDrawMiddle(screen->w*5/7,pos,0,item->name,font);
		pos+=font->maxheight;
		item = item->next;
	}
	spFontDrawMiddle(screen->w*5/7,screen->h/4+(2+menu_count)*font->maxheight/2,0,SP_PRACTICE_OK_NAME": Choose    "SP_PRACTICE_CANCEL_NAME": Back",font);
	spFlip();
}

int calc_menu(Uint32 steps)
{
	if (spGetInput()->axis[1] < 0)
	{
		spGetInput()->axis[1] = 0;
		if (menu_sel > 0)
			menu_sel--;
	}
	if (spGetInput()->axis[1] > 0)
	{
		spGetInput()->axis[1] = 0;
		if (menu_sel < menu_count-1)
			menu_sel++;
	}
	if (spGetInput()->button[SP_PRACTICE_OK])
	{
		spGetInput()->button[SP_PRACTICE_OK] = 0;
		pMenuItem item = firstMenuItem;
		int i;
		for (i = 0;i < menu_sel; i++)
			item = item->next;
		if (item->callBack)
			item->callBack();
		return 1;
	}
	return 0;
}

void run_menu()
{
	spLoop(draw_menu,calc_menu,10,resize,NULL);
}

void delete_menu()
{
	while (firstMenuItem)
	{
		pMenuItem next = firstMenuItem->next;
		free(firstMenuItem);
		firstMenuItem = next;
	}
	menu_sel = 0;
	menu_count = 0;
}

void add_menu(char* name,void ( *callBack )( void ))
{
	pMenuItem item = (pMenuItem)malloc(sizeof(tMenuItem));
	sprintf(item->name,"%s",name);
	item->callBack = callBack;
	item->next = firstMenuItem;
	firstMenuItem = item;
	menu_count++;
}

void menu_exit()
{
	exit_now = 1;
}

void main_menu()
{
	sprintf(menu_name,"Main menu");
	add_menu("Exit",menu_exit);
	add_menu("Save as",NULL);
	add_menu("Save",NULL);
	add_menu("Load",NULL);
	run_menu();
	delete_menu();
}

void options_menu()
{
	
}
