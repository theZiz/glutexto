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
spFileListPointer dialog_list = NULL;
spFileListPointer dialog_list_mom = NULL;

#define DIALOG_SHOW_BEFORE 4

void dialog_test_and_add(int* pos,spFileListPointer mom,int special)
{
	if (mom == NULL)
		return;
	char buffer[512];
	int i;
	for (i = strlen(mom->name)-1; i >= 0; i--)
	{
		if (mom->name[i] == '/')
			break;
	}
	i++;
	char filename[512];
	if (mom->type == SP_FILE_DIRECTORY)
		sprintf(filename,"[d] %s",&(mom->name[i]));
	else
		sprintf(filename,"[f] %s",&(mom->name[i]));
	if (special)
		sprintf(buffer,"> %s <",filename);
	else
		sprintf(buffer,"%s",filename);
	spFontDraw(5,*pos,0,buffer,fontInverted);
	if (special)
		spLine(5,*pos+fontInverted->maxheight-1,0,5+spFontWidth(buffer,font),*pos+fontInverted->maxheight-1,0,EDIT_TEXT_COLOR);		
	*pos+=fontInverted->maxheight+1;
}

void draw_dialog()
{
	//draw files
	spSelectRenderTarget(editSurface);
	spClearTarget( EDIT_BACKGROUND_COLOR );
	spFileListPointer before[DIALOG_SHOW_BEFORE];
	memset( before, 0, sizeof(spFileListPointer)*DIALOG_SHOW_BEFORE);
	
	int i;
	spFileListPointer mom = dialog_list_mom;
	for( i = DIALOG_SHOW_BEFORE-1; i >=0; i--)
	{
		mom = mom->prev;
		if (mom == NULL)
			break;
		before[i] = mom;
	}
	
	int pos = 5;
	for ( i = 0; i < DIALOG_SHOW_BEFORE; i++)
		dialog_test_and_add(&pos,before[i],0);
	dialog_test_and_add(&pos,dialog_list_mom,1);
	mom = dialog_list_mom->next;
	while (mom && pos < editSurface->h-fontInverted->maxheight)
	{
		dialog_test_and_add(&pos,mom,0);
		mom = mom->next;
	}
	
	
	//drawing all
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spBlitSurfacePart( screen->w/2,(screen->h+fontInverted->maxheight)/2,0,editSurface,0,0,editSurface->w,editSurface->h-fontInverted->maxheight-2);
	switch (dialog_kind)
	{
		case 0:
			spFontDrawMiddle(screen->w/2,0,0,"Choose file to load",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose [f]ile or enter [d]irectory    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
		case 1:
			spFontDrawMiddle(screen->w/2,0,0,"Choose location to save",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose or enter [d]irectory    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
		case 2:
			spFontDrawMiddle(screen->w/2,0,0,"Choose filename to save",font);
			spFontDrawMiddle(screen->w/2,screen->h-font->maxheight,0,SP_PRACTICE_OK_NAME": Choose letter    "SP_PRACTICE_CANCEL_NAME": Cancel",font);
			break;
	}
	char buffer[256];
	if (strcmp(dialog_folder,"/") == 0)
		sprintf(buffer,"Folder:   /");
	else
		sprintf(buffer,"Folder:   %s/",dialog_folder);
	spFontDrawMiddle(screen->w/2,font->maxheight,0,buffer,font);
	
	spFlip();
}

int calc_dialog(Uint32 steps)
{
	if (spGetInput()->button[SP_PRACTICE_CANCEL])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL] = 1;
		return 1;
	}
	if (time_until_next > 0)
		time_until_next -= steps;
	if (spGetInput()->axis[1] < 0 && dialog_list_mom->prev)
	{
		if (time_until_next <= 0)
		{
			dialog_list_mom = dialog_list_mom->prev;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	if (spGetInput()->axis[1] > 0 && dialog_list_mom->next)
	{
		if (time_until_next <= 0)
		{
			dialog_list_mom = dialog_list_mom->next;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	{
		time_until_next = 0;
		next_in_a_row = 0;
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
	spFileGetDirectory( &dialog_list, dialog_folder, 0, 0);
	spFileSortList( &dialog_list, SP_FILE_SORT_BY_TYPE_AND_NAME);
	dialog_list_mom = dialog_list;
	spLoop(draw_dialog,calc_dialog,10,resize,NULL);
	spFileDeleteList(dialog_list);
}

void load_dialog()
{
	run_dialog(0);
}

void save_as_dialog()
{
	run_dialog(1);
}
