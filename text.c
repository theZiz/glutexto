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
  
int line_count = 0;
int text_changed = 0;
char last_filename[512] = "";

void clearText()
{
	while (text)
	{
		free(text->line);
		pText next = text->next;
		free(text);
		text = next;
	}
	line_count = 0;
	line_number = 0;
}

pText addTextLine(char* line,pText after)
{
	pText newText = (pText)malloc(sizeof(tText));
	newText->prev = after;
	if (after)
	{
		newText->next = after->next;
		if (after->next)
			after->next->prev = newText;
		after->next = newText;
	}
	else
	{
		newText->next = text;
		text = newText;
	}
	newText->length = strlen(line);
	newText->reserved = (newText->length+4)*5/4;
	newText->line = (char*)malloc(newText->reserved);
	memcpy(newText->line,line,newText->length+1);
	line_count++;
	return newText;
}

void addToLine(char* newBuffer)
{
	int l = strlen(newBuffer);
	if (l + momLine->length + 1 > momLine->reserved) //Realloc
	{
		momLine->reserved = (momLine->length + l +4)*5/4;
		char* new_reserved = (char*)malloc(momLine->reserved);
		memcpy(new_reserved,momLine->line,momLine->length+1);
		free(momLine->line);
		momLine->line = new_reserved;
	}
	char end_line[momLine->length-line_pos+1];
	memcpy(end_line,&(momLine->line[line_pos]),momLine->length-line_pos+1);
	sprintf(&(momLine->line[line_pos]),"%s%s",newBuffer,end_line);
	momLine->length += l;
	line_pos += l;
	text_changed = 1;
}

void removeFromLine()
{
	char end_line[momLine->length-line_pos+1];
	memcpy(end_line,&(momLine->line[line_pos]),momLine->length-line_pos+1);
	if (line_pos-1 < 0)
	{
		if (momLine->prev)
		{
			//Removing myself
			momLine->prev->next = momLine->next;
			if (momLine->next)
				momLine->next->prev = momLine->prev;
			free(momLine->line);
			pText prev = momLine->prev;
			free (momLine);
			momLine = prev;
			line_number--;
			line_count--;
			int l = momLine->length;
			line_pos = l;
			addToLine(end_line);
			line_pos = l;
		}
	}
	else
	{
		line_pos--;
		sprintf(&(momLine->line[line_pos]),"%s",end_line);
		momLine->length--;
		text_changed = 1;
	}
}


void newText()
{
	if (text_changed)
	{
		char buffer[1024];
		sprintf(buffer,"You didn't save\n%s.\nDo you really want to create\na new document?",last_filename);
		if (ask_yes_no(buffer) == 0)
			return;
	}
	clearText();
	momLine = addTextLine("\n",NULL);
	line_number = 1;
	line_pos = 0;
	text_changed = 0;
}

void loadText(char* filename)
{
	if (text_changed)
	{
		char buffer[1024];
		sprintf(buffer,"You didn't save\n%s.\nDo you really want to open\n%s?",last_filename,filename);
		if (ask_yes_no(buffer) == 0)
			return;
	}
	int i;
	for (i = strlen(filename)-1; i >= 0; i--)
		if (filename[i] == '/')
			break;
	i++;
	sprintf(last_filename,"%s",&filename[i]);
	printf("Loading %s...\n",filename);
	SDL_RWops *file=SDL_RWFromFile(filename,"r");
	if (file == NULL)
		return;
	clearText();
	char buffer[65536];
	pText last = text;
	while (spReadOneLine(file,buffer,65536) == 0)
		last = addTextLine(buffer,last);
	momLine = text;
	line_number = 1;
	line_pos = 0;
	SDL_RWclose(file);
	text_changed = 0;
}

void saveText(char* filename)
{
	printf("Saving %s...\n",filename);
	SDL_RWops *file=SDL_RWFromFile(filename,"w");
	if (file == NULL)
		return;
	pText line = text;
	while (line)
	{
		spWriteOneLine(file,line->line);
		line = line->next;
	}
	SDL_RWclose(file);
	text_changed = 0;
}
