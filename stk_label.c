#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "stk_base.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_base.h"
#include "stk_color.h"
#include "stk_label.h"
#include "stk_default.h"


static void STK_LabelCalculatePattern(STK_Label *label, SDL_Rect *rect);


STK_Label *STK_LabelNew(Uint16 x, Uint16 y, Uint16 width, Uint16 height, char *str, Uint32 flag)
{
	STK_Widget *widget;
	SDL_Rect rect;
	STK_Window *win;
	
	// here, check the validation of x and y
	win = STK_WindowGetTop();
	if (!win) 
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	STK_Label *label = (STK_Label *)STK_Malloc(sizeof(STK_Label));
	widget = (STK_Widget *)label;
	STK_WidgetInitInstance(widget);
	
	widget->name	= "Label";
	widget->type	= STK_WIDGET_LABEL;
	// can't get focus
	widget->flags	= 0;			
	widget->border 	= 0;
	widget->fixed	= (flag > 0? 1 : 0);
	
	rect.x	= x;
	rect.y 	= y;
	rect.w	= (width < STK_LABEL_DEFAULT_WIDTH? STK_LABEL_DEFAULT_WIDTH : width);
	rect.h	= (height < STK_LABEL_DEFAULT_HEIGHT? STK_LABEL_DEFAULT_HEIGHT : height);
	// create surface
	STK_WidgetSetDims(widget, &rect);

	label->lineskip	= 0;
	label->alignment = STK_LABEL_TOPLEFT;
	label->pattern	= STK_LABEL_NORMAL;

	if (str) {
		char *psrc = (char *)STK_Malloc(strlen(str) + 1);
		char *p, *saveptr;
		char **p1;
		int i, n;
		
		strcpy(psrc, str);
		
		n = 0;
		for (i=0; i<strlen(psrc); i++) {
			if (psrc[i] == '\n')
				n++; 
		}
		// single line
		if (n == 0) {
			label->caption = psrc;
			label->lines = 1;
		}
		// multiple line
		else {
			int t = 0;
			label->caption = (char *)STK_Malloc(strlen(str) + 1);
			strcpy(label->caption, psrc);
			
			label->pcaption = (char **)STK_Malloc(sizeof(char *) * (n+1));

			p1 = label->pcaption;
			p = strtok_r(psrc, "\n", &saveptr);
			while (p) {
				*p1++ = p;
				t++;
				p = strtok_r(NULL, "\n", &saveptr);
			}

			label->lines = t;
		}
		
		// adaptered to string
		if (!widget->fixed) {
			STK_LabelAdapterToString(label);
		}
	}
	else {
		label->caption = NULL;
	}
	
		
	
//for test	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x00, 0xff, 0xff);
	
	return label;
}

int STK_LabelRegisterType()
{
	STK_WidgetFuncs *f;
	STK_WidgetRegisterType( "Label", &f );
	f->draw = STK_LabelDraw;
	f->close = STK_LabelClose;
	
	return 0;
}


void STK_LabelDraw(STK_Widget *widget)
{
	SDL_Rect rect;
	STK_Label *label = (STK_Label *)widget;
	Uint32 tmpcolor;
	
	// first we need clear background before scaling.
	tmpcolor = STK_COLOR2INT(widget->surface, widget->bgcolor);
	SDL_FillRect(widget->surface, NULL, tmpcolor);

	// if label is extended, to adapter to the string
	if (!widget->fixed) {
		STK_LabelAdapterToString(label);

		// fill background
		// in STK_LabelAdapterToString, may alloc a new surface 
		//tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
		tmpcolor = STK_COLOR2INT(widget->surface, widget->bgcolor);
		SDL_FillRect(widget->surface, NULL, tmpcolor);
	}
	
	
	if (label->lines == 1 && label->caption) {
		// calculate the rect area of font surface: 
		// results are placed in rect as to alignment.
		STK_LabelCalculatePattern(label, &rect);
		
		// draw text using widget's fgcolor and (local background only for test colorkey)
		STK_FontDraw( STK_FontGetDefaultFontWithSize(), label->caption, widget, &rect, &widget->fgcolor, &widget->bgcolor );
	}
	else if (label->lines > 1 && label->pcaption) {
		int i;
		char **p1 = label->pcaption;
				
		for (i=0; i<label->lines; i++) {
			rect.x = 0;
			rect.y = i * (label->lineskip + label->line_height);
			rect.w = widget->rect.w;
			rect.h = label->line_height;
			
			STK_FontDraw( 	STK_FontGetDefaultFontWithSize(), 
					p1[i], 
					widget, 
					&rect, 
					&widget->fgcolor, 
					&widget->bgcolor );
		}
	
	}
	
	// till now, the surface of label has been filled, but shall we blit it to window surface?
	
}

int STK_LabelClose(STK_Widget *widget)
{
	STK_Label *label = (STK_Label *)widget;
	int i;
	
	// now ready to free surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	// now ready to free string
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	if (label->pcaption) {
		char **p1 = label->pcaption;
		for (i = 0; i<label->lines; i++) {
			if ((char *)p1[i]) {
				free(p1[i]);
				p1[i] = NULL;
			}
		}
		free(p1);
		label->pcaption = NULL;
	}
	// now ready to free label node
	free(label);
	
	return 0;
}

int STK_LabelAdapterToString(STK_Label *label)
{
	SDL_Rect rect;
	STK_Widget *widget = (STK_Widget *)label;
	
	// backup widget's rect
	STK_BaseRectCopy(&rect, &widget->rect);
	if (label->lines == 1) {
		// force rect to get eventual label->caption width and height 
		STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &rect, label->caption);
		label->line_height = rect.h;
	}
	else if (label->lines > 1) {
		Uint32 width, height;
		int i;
		char **p1;

		p1 = label->pcaption;
		STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &rect, p1[0]);
	
		width = rect.w;
		label->line_height = rect.h;
		height = rect.h * label->lines + label->lineskip * (label->lines - 1);

		for (i=1; i<label->lines; i++) {
			STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &rect, p1[i]);
			if (width < rect.w)
				width = rect.w;
		}
		
		rect.w = width;
		rect.h = height;	
	}
		
	// if areas don't equal, need to reset dimensions of this 
	//   label: free previous surface and alloc a new one
 	if (!STK_BaseRectEqual(&rect, &widget->rect))
		STK_WidgetSetDims(widget, &rect);

	return 0;
}

static void STK_LabelCalculatePattern(STK_Label *label, SDL_Rect *rect)
{
	STK_Widget *widget = (STK_Widget *)label;
	SDL_Rect dims = {0};
	int dw, dh;
	
	/* Calculate the total size of the string in pixels */
	STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &dims, label->caption);
	dw = widget->rect.w - dims.w;
	dh = widget->rect.h - dims.h;
	// if widget is too small, limit the drawing area in widget->rect
	if (dw <= 0 || dh <= 0) {
		rect->x = 0;
		rect->w = widget->rect.w;
		rect->y = 0;
		rect->h = widget->rect.h;
		return;
	}
	
	switch (label->alignment) {
	case STK_LABEL_TOPLEFT:
		rect->x = 0;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;		
	case STK_LABEL_TOPCENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2 + 1;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case STK_LABEL_CENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case STK_LABEL_CENTERLEFT:
		rect->x = 0;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;			
	}
}

int STK_LabelSetAlignment(STK_Label *label, int alignment)
{
	if (!label)
		return -1;
	
	label->alignment = alignment;
	
	return 0;
}

int STK_LabelSetText(STK_Label *label, char * text)
{
	STK_Widget *widget = (STK_Widget *)label;
	
	if (!label || !text)
		return -1;
	
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	// because the c string have a zero char in the end, malloc text size + 1 
	label->caption = (char *)STK_Malloc(strlen(text) + 1);
	// after this copy, the last char left in caption is 0
	strcpy(label->caption, text);
	
	if (!widget->fixed)
		STK_LabelAdapterToString(label);
	
	STK_WidgetEventRedraw((STK_Widget *)label);
			
	return 0;
}

char *STK_LabelGetText(STK_Label *label)
{
	return label->caption;
}


int STK_LabelSetColor(STK_Label *label, int which, Uint8 r, Uint8 g, Uint8 b)
{
	STK_Widget *widget = (STK_Widget *)label;
	
	if (!widget) 
		return -1;
		
	switch (which) {
		case STK_COLOR_FOREGROUND:
			STK_BaseColorAssign(&widget->fgcolor, r, g, b);
			break;
		case STK_COLOR_BACKGROUND:
			STK_BaseColorAssign(&widget->bgcolor, r, g, b);
			break;
		default:
			break;
	}
	
	// need to submit a redraw event
	STK_WidgetEventRedraw(widget);

	return 0;
}

