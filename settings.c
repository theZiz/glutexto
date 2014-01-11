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

#ifdef GCW
char* get_path(char* buffer,char* file)
{
	sprintf(buffer,"%s/.config/glutexto",getenv("HOME"),file);
	spCreateDirectoryChain(buffer);
	sprintf(buffer,"%s/.config/glutexto/%s",getenv("HOME"),file);
	return buffer;
}
#else
char* get_path(char* buffer,char* file)
{
	sprintf(buffer,".%s",file);
	return buffer;
}
#endif

void load_settings()
{
	char buffer[256];
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"settings.ini"),"r");
	if (file == NULL)
	{
		while (selectedFont)
		{
			if (strcmp(selectedFont->name,"LiberationSans") == 0)
				break;
			selectedFont = selectedFont->next;
		}
		if (selectedFont == NULL)
			selectedFont = firstFont;
		return;
	}
	while (spReadOneLine(file,buffer,256) == 0)
	{
		char* value = strchr(buffer,':');
		if (!value)
			continue;
		value+=2;
		if (strstr(buffer,"Font: ") == buffer)
		{
			selectedFont = firstFont;
			while (selectedFont && strcmp(value,selectedFont->name) != 0)
				selectedFont = selectedFont->next;
			if (selectedFont == NULL)
				selectedFont = firstFont;
		}
		if (strstr(buffer,"Show Lines: ") == buffer)
			showLines = (atoi(value) != 0);
		if (strstr(buffer,"Font Size: ") == buffer)
		{
			fontSize = atoi(value);
			if (fontSize < MIN_FONT_SIZE)
				fontSize = MIN_FONT_SIZE;
			if (fontSize > MAX_FONT_SIZE)
				fontSize = MAX_FONT_SIZE;
		}
	}
	
	SDL_RWclose(file);	
}

void save_settings()
{
	char buffer[256];
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"settings.ini"),"w");
	sprintf(buffer,"Font: %s",selectedFont->name);
	spWriteOneLine(file,buffer);
	sprintf(buffer,"Font Size: %i",fontSize);
	spWriteOneLine(file,buffer);
	sprintf(buffer,"Show Lines: %i",showLines);
	spWriteOneLine(file,buffer);
	SDL_RWclose(file);
}
