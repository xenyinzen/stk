#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>

#include "stk_mm.h"
#include "stk_text.h"
#include "stk_signal.h"
#include "stk_widget.h"
#include "stk_entry.h"

#define STK_ENTRY_BORDER_THICKNESS	3
#define STK_ENTRY_CURSOR_WIDTH		2

static void STK_EntryEventKeyDown(STK_Object *object, void *signaldata, void *userdata);

void STK_EntryDrawCursor(STK_Entry *entry);
int STK_EntryCalcTextareaRect(STK_Entry *entry);
int STK_EntryKeyGeneral(STK_Entry *entry);
int STK_EntryKeyBackspace(STK_Entry *entry);

STK_Widget *STK_EntryNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint16 limited_num, char *initial_str)
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
        widget->flags |= WIDGET_FOCUSABLE;

        entry->text = STK_TextNew(initial_str);
	entry->fgcolor = 0x00404040;
	entry->bgcolor = 0x00f0f0f0;
	
	STK_EntryCalcTextareaRect(entry);

	// connect the keydown event
	STK_SignalConnect(widget, "keydown", STK_EntryEventKeyDown, widget);
			
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

void STK_EntryClose(STK_Widget *widget)
{
	STK_Entry *entry = (STK_Entry *)widget;
 
        // now ready to free surface
        if (widget->surface) {
                SDL_FreeSurface(widget->surface);
        }
        
       	if (entry->text) {
       		STK_TextFree(entry->text);
       	}
        
        // now ready to free slidebar structure  
        free(entry);
}

void STK_EntryDraw(STK_Widget *widget)
{
	STK_Entry *entry = (STK_Entry *)widget;
        SDL_Color fg = {0};
        SDL_Color bg = {0};
	SDL_Rect r;
	
	// fillup background
	SDL_FillRect(widget->surface, NULL, 0x00f0f0f0);

	// draw texts, draw all once
//	if (entry->text) {
		// calculate the rect area of font surface: results are placed in rect.
                fg.r = (Uint8)((entry->fgcolor >> 16) & 0xff);
                fg.g = (Uint8)((entry->fgcolor >> 8) & 0xff); 
                fg.b = (Uint8)((entry->fgcolor >> 0) & 0xff); 
                
                bg.r = (Uint8)((entry->bgcolor >> 16) & 0xff);
                bg.g = (Uint8)((entry->bgcolor >> 8) & 0xff); 
                bg.b = (Uint8)((entry->bgcolor >> 0) & 0xff); 
		
		r.x = STK_ENTRY_BORDER_THICKNESS;
		r.y = STK_ENTRY_BORDER_THICKNESS;
		r.w = 0;
		r.h = 0;
		
                STK_FontDraw(widget, entry->text->data, &r, &fg, &bg);
		
//	}
	STK_FontAdapter(&r, entry->text->data);
	entry->cursor_pos = r.w + STK_ENTRY_BORDER_THICKNESS;
	entry->cursor_height = r.h;
	// fillup cursor: need to clean previous black line and draw new line in new position
	// every time need to redraw cursor, but not background, and chars on textarea 
	STK_EntryDrawCursor(entry);
}

void STK_EntryDrawCursor(STK_Entry *entry)
{
	STK_Widget *widget = (STK_Widget *)entry;
	SDL_Rect r;
	
	r.x = entry->cursor_pos;
	r.y = STK_ENTRY_BORDER_THICKNESS;
	r.w = 2;
	r.h = entry->cursor_height;
	//
	// ......
	//
	SDL_FillRect(widget->surface, &r, 0x00000000);
	
}


// process keydown event
static void STK_EntryEventKeyDown(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Entry *entry = (STK_Entry *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	STK_Widget *widget = (STK_Widget *)object;
	SDLMod mod;
	int key;
	
	// have no focus, return immediately
	// this is very important, to keep only one input entry at any time
//	if (!STK_WidgetHasFocus(widget))
//		return;
		
	mod = event->key.keysym.mod;
	switch (event->key.keysym.sym) {
	case SDLK_BACKSPACE:
		if (entry->text->length > 0) {
			STK_EntryKeyBackspace(entry);	
			STK_SignalEmit(widget, "changed", NULL);
		}
    		break;
	case SDLK_RETURN:
		if (widget)
			STK_SignalEmit(widget, "activate", NULL);
		break;	
	default:
		// input general char		
		key = event->key.keysym.unicode;
		// numeric keypad translation
		if ((key >= SDLK_KP0) && (key <= SDLK_KP9))
			key -= 0xd0;
		if ((key >= 0x20) && (key <= 0xff)) {
			// process Capital alphanumber
			if (mod & KMOD_SHIFT) {
				// a-z, and A-Z
				if (key >= SDLK_a && key <= SDLK_z)
					key -= 32;
				// :, and ;
				if (key == SDLK_SEMICOLON)
					key = SDLK_COLON;
			}
			
			// insert char to text.data
			if (key < 127) {
				// now, only process ASCII char
				entry->inputkey[0] = key;
				STK_EntryKeyGeneral(entry);
				STK_SignalEmit(widget, "changed", NULL);
			}
		
		}
	        break;
	}
	STK_WidgetEventRedraw(widget);

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

// input a char, shift display window and redraw window
int STK_EntryKeyGeneral(STK_Entry *entry)
{
	// insert input key into text buffer
	STK_TextAppendStr(entry->text, entry->inputkey);

	return 0;
}

int STK_EntryKeyBackspace(STK_Entry *entry)
{
	// remove the last ASCII char from text buffer
	STK_TextRemoveStr(entry->text, 1);
	
	return 0;
}

