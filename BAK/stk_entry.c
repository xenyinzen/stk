#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>

#include "stk_mm.h"
#include "stk_text.h"
#include "stk_entry.h"

#define STK_ENTRY_BORDER_THICKNESS	3
#define STK_ENTRY_CURSOR_WIDTH		2


static char utf8char[4] = {0};

STK_Widget *STK_EntryNew(char *initial_str, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
        STK_Widget *widget;
        STK_Entry *entry = (STK_Entry *)STK_Malloc(sizeof(STK_Entry));

        // here, need to check x and y's reasonablity

        widget = (STK_Widget *)entry;
        widget->name    = "Entry";
        widget->type    = STK_WIDGET_ENTRY;
        widget->rect.x  = x;
        widget->rect.y  = y;
        widget->rect.w  = w;
        widget->rect.h  = h;
        widget->flags   = 0;

        if (str) {
        	// alloc a duplication of initial_str
                entry->text.data = strdup(initial_str);
                entry->text.length = strlen(initial_str);
                entry->text.size = strlen(initial_str);
	}
	else {
		entry->text.data = NULL;
		entry->text.length = 0;
		entry->text.size = 0;
	}
			
	entry->fgcolor = 0x00000000;
	entry->bgcolor = 0x00f0f0f0;

	STK_SignalConnect(widget,"keydown",STK_EntrySignalKeyDown, widget);
			
	return widget;
}

int STK_EntryRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "Entry", &f );
        f->draw = STK_EntryDraw;
        f->close = STK_EntryClose;

        return 0;
}

// 
void STK_EntryDraw(STK_Widget *widget)
{
	STK_Entry *entry = (STK_Entry *)widget;
	Uint32 start_char;
	Uint32 i = 0;
	Uint32 dist;
	SDL_Rect r;
	
	// fillup background
	SDL_FillRect(widget->surface, NULL, 0x00f0f0f0);
	
	// fillup font surface (one by one), control the char dividing space by myself
	for (i = 0; i < entry->start_char; i++) {
		int l = 0;
		l = entry->start_char + i;
		// find proper char into utf8char array: can process ascii char and Chinese character
		STK_TextGetChar(utf8char, entry->text, l);
		
		// initial value is 2, for cursor
		dist = STK_EntryGetDistance(entry);
		// calculate the relative location of the drawing char rect
		r.x = STK_ENTRY_BORDER_THICKNESS + dist;
		r.y = STK_ENTRY_BORDER_THICKNESS;
		STK_FontGetCharSize(entry->text.data[l], &r); // TTF_SizeUTF8(entry->text.data[entry->cur_pos]);
		
		// fill char on the widget's surface
		STK_FillChar(widget->surface, &r, utf8char);
		// update distance
		dist = dist + r.w + entry->charskip;
		STK_EntrySetDistance(dist);
		
		// update current cursor position
		entry->cur_pos = l;		
	}
	
	// fillup cursor
	STK_EntryDrawCursor(entry);
}


void STK_EntryDrawCursor(STK_Entry *entry)
{
	SDL_Rect r;
	STK_Widget *widget = (STK_Widget *)widget;
	// according to current cursor location, but now it's the last character
	int dist = STK_EntryGetDistance();
	
	r.x = STK_ENTRY_BORDER_THICKNESS + dist - STK_ENTRY_CURSOR_WIDTH;
	r.y = STK_ENTRY_BORDER_THICKNESS;
	r.w = STK_ENTRY_CURSOR_WIDTH;			// cursor width
	r.h = widget->rect.h - 2 * STK_ENTRY_BORDER_THICKNESS;

	// don't blind now
	SDL_FillRect(widget->surface, &r, entry->cursor_color);
}

// the distance of a new char will been drawn
Uint32 STK_EntryGetDistance(STK_Entry *entry)
{
	// calculate the distance from the start of string.
	// I think: should record.
	int dist = STK_ENTRY_CURSOR_WIDTH;
	
	for (i = entry->start_char; i < entry->cur_pos; i++) {
		dist += charwidth[entry->text.data[i]] + STK_ENTRY_CURSOR_WIDTH;		
	}
	
	return dist;
}

// need to do a initial function to charwidth
int STK_EntryInitCharWidth(STK_Widget *widget)
{
	char i = 0;
	int w, h;
	STK_Entry *entry = (STK_Entry *)widget;
	
	for (i=0; i<256; i++)
		entry->charwidth[i] = 0;
	
	for (i = 0; i < 127; i++) {
		if (isprint(i)) {
			utf8char[0] = i;
			utf8char[1] = 0;
			TTF_SizeUTF8(font, utf8char, &w, &h);
			// update ASCII char's width
			entry->charwidth[i] = w;
		}		
	}
	
	// the utf8 code of 'wo' in Chinese
	utf8char[0] = 0xe6;
	utf8char[1] = 0x88;
	utf8char[2] = 0x91;
	utf8char[3] = 0;
	TTF_SizeUTF8(font, utf8char, &w, &h);
	// we use charwidth[127] to store Chinese character
	entry->charwidth[127] = w;	

}

// process keydown event
static void STK_EntrySignalKeyDown(STK_Object *object,void *signaldata,void *userdata)
{
	STK_Entry *entry = (STK_Entry *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	STK_Widget *widget = (STK_Widget *)object;
	SDLMod mod;
	int key;
	
	// have no focus, return immediately
	// this is very important, to keep only one input entry at any time
	if (!STK_WidgetHasFocus(widget))
		return;
		
	mod = event->key.keysym.mod;
	switch (event->key.keysym.sym) {
	case SDLK_DELETE:
		// if (entry->cur_pos )
		STK_EntryKeyDelete();
		STK_WidgetEventRedraw(widget);
		STK_SignalEmit(widget, "changed", NULL);
		break;
	case SDLK_BACKSPACE:
		if (entry->cur_pos > 0) {
			STK_EntryKeyBackspace(entry);	
			STK_WidgetEventRedraw(widget);
			STK_SignalEmit(widget, "changed", NULL);
		}
    		break;
    	case SDLK_HOME:
    		STK_EntryKeyHome(entry);
    		STK_WidgetEventRedraw(widget);
    		break;
    	case SDLK_END:
		STK_EntryKeyEnd(entry);
		STK_WidgetEventRedraw(widget);
		break;
	case SDLK_LEFT:
		if (entry->cur_pos > 0) {
			STK_EntryKeyLeft(entry);
			STK_WidgetEventRedraw(widget);
		}    			
		break;    		
	case SDLK_RIGHT:
		if () {
			STK_EntryKeyRight(entry);
			STK_WidgetEventRedraw(widget);
		}
		break;
	case SDLK_RETURN:
		if (STK_WidgetHasFocus(widget))
			STK_SignalEmit(widget. "activate", NULL);
		break;	
	default:
		// input general char		
		key = event->key.keysym.unicode;
		// numeric keypad translation
		if ((key >= SDLK_KP0) && (key <= SDLK_KP9))
			key -= 0xd0;
		if ((key >= 0x20) && (key <= 0xff)) {
			if (mod & KMOD_SHIFT) {
				if (key >= SDLK_a && key <= SDLK_z)
					key -= 32;
				if (key == SDLK_SEMICOLON)
					key = SDLK_COLON;
			}
			
			if (key < 127) {
				STK_EntryKeyGeneral();
				STK_WidgetEventRedraw(widget);
				STK_SignalEmit(widget, "changed", NULL);
			}
		
		}
	        break;
	}

	return ;
}

int STK_EntryCalcTextareaRect(STK_Entry *entry)
{
	entry->textarea.x = entry->widget.rect.x + STK_ENTRY_BORDER_THICKNESS + 1;
	entry->textarea.y = entry->widget.rect.y + STK_ENTRY_BORDER_THICKNESS + 1;
	entry->textarea.w = entry->widget.rect.w - 2*(STK_ENTRY_BORDER_THICKNESS + 1);
	entry->textarea.h = entry->widget.rect.h - 2*(STK_ENTRY_BORDER_THICKNESS + 1);
	
	return 0;
}

Uint32 STK_EntryCalcDisplayWindowTextLength(STK_Entry *entry)
{
	int i = 0;
	Uint32 sum = 0;
	
	Uint32 start_char = entry->start_char;
	Uint32 end_char  = entry->end_char;
	// here only process english char now
	for (i=0; i <= end_char - start_char + 1; i++) {
		sum += charwidth[entry->text->data[start_char + i]]
	}

	return sum;
}

// in this function, only modify display window about variables
int STK_EntryKeyLeft(STK_Entry *entry)
{
	Uint32 tmp_length;
	
	if (entry->cur_pos > 0)
		entry->cur_pos--;
	else {
		if (entry->start_char > 0)
			entry->start_char--;
		tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
		// here to make sure that text display won't excced entry's textarea border
		while (tmp_length > entry->textarea.w) {
			entry->end_char--;
			tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
		}
	}
	
	return 0;
}

int STK_EntryKeyRight(STK_Entry *entry)
{
	Uint32 tmp_num = entry->end_char - entry->start_char + 1;
	Uint32 tmp_length;
	
	if (entry->cur_pos < tmp_num)
		entry->cur_pos++;
	else {
		if (entry->end_char < entry->text.length - 1)
			entry->end_char++;
		tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
		// here to make sure that text display won't excced entry's textarea border
		while (tmp_length > entry->textarea.w) {
			entry->start_char++;
			tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
		}
	}
	
	return 0;
}

int STK_EntryKeyHome(STK_Entry *entry)
{
	entry->start_char = 0;
	
	tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
	// here to make sure that text display won't excced entry's textarea border
	while (tmp_length > entry->textarea.w) {
		entry->end_char--;
		tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
	}

	entry->cur_pos = 0;
	return 0;
}


int STK_EntryKeyEnd(STK_Entry *entry)
{
	entry->end_char = entry->text.length - 1;
	
	tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
	// here to make sure that text display won't excced entry's textarea border
	while (tmp_length > entry->textarea.w) {
		entry->start_char++;
		tmp_length = STK_EntryCalcDisplayWindowTextLength(entry);
	}

	entry->cur_pos = entry->end_char - entry->start_char;
	return 0;
}

int STK_EntryKeyDelete(STK_Entry *entry)
{
	


}



