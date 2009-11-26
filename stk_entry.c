#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_mm.h"
#include "stk_text.h"
#include "stk_signal.h"
#include "stk_base.h"
#include "stk_image.h"
#include "stk_color.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_entry.h"

#define STK_ENTRY_BORDER_THICKNESS	3
#define STK_ENTRY_CURSOR_WIDTH		2

static void STK_EntryEventKeyDown(STK_Object *object, void *signaldata, void *userdata);

void STK_EntryDrawCursor(STK_Entry *entry);
int STK_EntryCalcTextareaRect(STK_Entry *entry);
int STK_EntryKeyGeneral(STK_Entry *entry);
int STK_EntryKeyBackspace(STK_Entry *entry);

STK_Entry *STK_EntryNew(char *initial_str, Uint16 limited_num, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
        STK_Entry *entry;
        STK_Widget *widget;
        
        STK_Window *win;
        SDL_Rect rect;
        int height;
        
        win = STK_WindowGetTop();
        if (!win)
        	return -1;
        if (x >= win->widget.rect.w || y >= win->widget.rect.h)
        	return -1;
        
        entry = (STK_Entry *)STK_Malloc(sizeof(STK_Entry));
        widget = (STK_Widget *)entry;
	STK_WidgetInitInstance(widget);
	
        widget->name    = "Entry";
        widget->type    = STK_WIDGET_ENTRY;
        widget->flags  |= WIDGET_FOCUSABLE;
	widget->fixed	= 1;
	widget->border 	= STK_ENTRY_BORDER_THICKNESS;
	STK_BaseColorAssign(&widget->bgcolor, STK_COLOR_ENTRY_BACKGROUND);
	STK_BaseColorAssign(&widget->fgcolor, STK_COLOR_ENTRY_FOREGROUND);
	
	height = STK_FontGetHeight(STK_FontGetDefaultFontWithSize()) + 2 * widget->border;
	
	STK_BaseRectAssign(&rect, x, y, w, height);
	STK_WidgetSetDims(widget, &rect);

	// connect the keydown event
	STK_SignalConnect(widget, "keydown", STK_EntryEventKeyDown, widget);
	
        entry->text = STK_TextNew(initial_str);
	if (limited_num > 0)
		entry->limits = limited_num;
	else
		entry->limits = 0;
	
	
	// assign textarea, record relative values
	STK_BaseRectAssign( &entry->textarea,
				widget->border,
				widget->border,
				widget->rect.w - 2 * (widget->border),
				widget->rect.h - 2 * (widget->border)  );
			
	entry->cursor_height = entry->textarea.h;

	return entry;
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
        
        return;
}

void STK_EntryDraw(STK_Widget *widget)
{
	STK_Entry *entry = (STK_Entry *)widget;
	SDL_Rect r;
	Uint32 tmpcolor;
	
	// fillup background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, NULL, tmpcolor);
	STK_ImageDrawFrame(widget->surface, STK_IMAGE_FRAME_SINGLELINE);
	
	// draw text
	if ( entry->text && entry->text->data && entry->text->length > 0)
		STK_FontDraw(	STK_FontGetDefaultFontWithSize(), 
			entry->text->data, 
			widget, 
			&entry->textarea, 
			&widget->fgcolor, 
			&widget->bgcolor);
	
	// calculate the end pixels of the text	
	if (STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &r, entry->text->data) == 0)
		entry->cursor_pos = r.w + entry->textarea.x;
	else
		entry->cursor_pos = entry->textarea.x;

	// fillup cursor: need to clean previous black line and draw new line in new position
	// every time need to redraw cursor, but not background, and chars on textarea 
	STK_EntryDrawCursor(entry);
	
	return;
}

void STK_EntryDrawCursor(STK_Entry *entry)
{
	STK_Widget *widget = (STK_Widget *)entry;
	SDL_Rect r;
	
	STK_BaseRectAssign(&r, entry->cursor_pos, widget->border, 2, entry->cursor_height);
	SDL_FillRect(widget->surface, &r, SDL_MapRGB(widget->surface->format, STK_COLOR_ENTRY_CURSOR));
	
	return;
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
				if (entry->text->length < entry->limits) {
					STK_EntryKeyGeneral(entry);
					STK_SignalEmit(widget, "changed", NULL);
				}
			}
		
		}
	        break;
	}
	STK_WidgetEventRedraw(widget);

	return ;
}

// input a char, shift display window and redraw window
int STK_EntryKeyGeneral(STK_Entry *entry)
{
	STK_Widget *widget = (STK_Widget *)entry;
	SDL_Rect r;
	
	// insert input key into text buffer
	STK_TextAppendStr(entry->text, entry->inputkey);

	// check if excced the edge of textarea
	STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &r, entry->text->data);
	if (widget->fixed && r.w > entry->textarea.w)
		STK_TextRemoveStr(entry->text, 1);

	return 0;
}

int STK_EntryKeyBackspace(STK_Entry *entry)
{
	// remove the last ASCII char from text buffer
	STK_TextRemoveStr(entry->text, 1);
	
	return 0;
}

