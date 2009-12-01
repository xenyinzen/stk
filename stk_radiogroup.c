#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_base.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"
#include "stk_default.h"


static void STK_RadioGroupEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata);


//
// char *radiostr[] = {"A", "AB", "ABC", "ABCD", "ABDEFGFFDSF"};
//
STK_RadioGroup *STK_RadioGroupNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *radiostr[], int num)
{	
	STK_RadioGroup *rg;
	STK_RadioButton *rb;
	STK_RadioButtonListNode *rbln, *ahead;
	STK_Widget *widget;
	STK_Window *win;
	SDL_Rect rect;
	int i = 0;

	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	rg = (STK_RadioGroup *)STK_Malloc(sizeof(STK_RadioGroup));
	widget = (STK_Widget *)rg;
	STK_WidgetInitInstance(widget);
	
	widget->name = "RadioGroup";
	widget->type = STK_WIDGET_RADIOGROUP;
	widget->flags |= WIDGET_FOCUSABLE;
	widget->border = STK_RADIOGROUP_BORDER_THICKNESS;
	widget->fixed = 0;
	widget->showborder = 1;

	STK_BaseRectAssign(&rect, x, y, w, h);
	STK_WidgetSetDims(widget, &rect);
		
	rg->n = num;
	rg->name = radiostr;
	rg->interval = 2 * widget->border;
	rg->nchoice = 0;
	rg->mode = 1;			// multiple choices
	rg->item_height = STK_RADIOBUTTON_DEFAULT_HEIGHT;

	STK_SignalConnect(widget, "mousebuttondown", STK_RadioGroupEventMouseButtonDown, widget);
	
	// create sub widgets of radio group: single list
	for (i=0; i<num; i++) {
		// create sub widget list node
		ahead = (STK_RadioButtonListNode *)STK_Malloc(sizeof(STK_RadioButtonListNode));
		
		// here, we think the height of the font is 30, it must be a global variable
		// here, we know, the coordination of those child radio buttons are relative to window (absoluted)
		// , but not radiogroup right now, may be we need have a filter procedure to change this
		rb = (STK_RadioButton *)STK_RadioButtonNew(
						x + widget->border, 
						y + widget->border + i*(rg->item_height + rg->interval),
						STK_RADIOBUTTON_DEFAULT_WIDTH,
						rg->item_height,
						radiostr[i]);
		// put radio button object into each list node
		ahead->rb = rb;
		ahead->i = i;
		ahead->next = NULL;
		// if first sub widget
		if (rg->rblist_head == NULL) {
			rg->rblist_head = ahead;
		}
		// else add to the end of list
		else {
			rbln->next = ahead;
		}
		rbln = ahead;
	}
	
	if (!widget->fixed)
		STK_RadioGroupAdapterToChild(rg); 
	
	return rg;
}

int STK_RadioGroupAdapterToChild(STK_RadioGroup *rg)
{	
	STK_RadioButtonListNode *child_list = rg->rblist_head;
	STK_Widget *widget = (STK_Widget *)rg;
	STK_Widget *child_widget;

	SDL_Rect rect, rect_B;
	int i;
	Uint16 width, height;
	
	// if have nothing to draw
	if (!rg->rblist_head)
		return -1;
	
	child_widget = rg->rblist_head->rb;
	// we must calculate the max length of child radio buttons
	width = child_widget->rect.w;
	rg->item_height = child_widget->rect.h;

	while (child_list) {
		// if child_list if true, we consider child_list->rb is also true
		child_widget = child_list->rb;
		if (child_widget->rect.w > width)
			width = child_widget->rect.w;
		
		// go next
		child_list = child_list->next;
	}
	
	// calculate total height
	height = rg->item_height * rg->n + widget->border * 2 + rg->interval * (rg->n - 1);
	
	widget->rect.w = width + widget->border * 2;
	widget->rect.h = height;

	STK_WidgetSetDims(widget, &widget->rect);	

/*	// scale all radiobutton's width to this maxmun width, not useful, because radiobutton will autoscale
	// when draw in STK_RadioButtonDraw
	child_list = rg->rblist_head;
	while (child_list) {
		// if child_list if true, we consider child_list->rb is also true
		child_widget = (STK_Widget *)child_list->rb;
		child_widget->rect.w = width;
		// recalc the y location of a radiobutton
		child_widget->rect.y = widget->rect.y + widget->border
					 + child_list->i * (rg->interval + rg->item_height);	
		// do really scaling action
		STK_WidgetSetDims(child_widget, &child_widget->rect);
		// go next
		child_list = child_list->next;
	}
*/	
	return 0;
}

void STK_RadioGroupDraw(STK_Widget *widget)
{
	STK_RadioGroup *rg = (STK_RadioGroup *)widget;
	
	if (!rg->rblist_head)
		return;

	// Because RadioGroup have relative fixed size to label and radiobutton, so need not adapter every time	
//	if (!widget->fixed)
//		STK_RadioGroupAdapterToChild(rg);

	if (widget->showborder)
		STK_ImageDrawFrame(widget->surface, STK_IMAGE_FRAME_SINGLELINE);
	
	// fill child radio buttons
	STK_RadioGroupFilling(rg);

}

void STK_RadioGroupClose(STK_Widget *widget)
{
	STK_RadioGroup *rg = (STK_RadioGroup *)widget;
	STK_RadioButtonListNode *child_listnode = rg->rblist_head;
	STK_RadioButtonListNode *ahead;
	STK_Widget *child_widget;
	
	F_Widget_Close close;
	
	if (rg->rblist_head) {
		// release child widgets
		while (child_listnode) {
			if (child_listnode->rb) {
				child_widget = (STK_Widget *)child_listnode->rb;
			
				close = STK_WidgetGetClose(child_widget);
				if (close) {
					close(child_widget);
					child_listnode->rb = NULL;					
				}
			}
			child_listnode = child_listnode->next;
		}
	
		// release child list: single list
		child_listnode = rg->rblist_head;
		ahead = child_listnode;
		// release from front to end
		while (ahead) {
			ahead = ahead->next;
			free(child_listnode);
			child_listnode = ahead;
		}
		rg->rblist_head = NULL;
	}
	
	// free radio group's surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	
	// free STK_RadioGroup structure
	free(rg);
	
	return 0;
}

int STK_RadioGroupRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "RadioGroup", &f );
        f->draw = STK_RadioGroupDraw;
        f->close = STK_RadioGroupClose;

        return 0;
}


int STK_RadioGroupFilling(STK_RadioGroup *rg)
{
	STK_Widget *widget = (STK_Widget *)rg;
	STK_RadioButtonListNode *child_list = rg->rblist_head;
	STK_Widget *child_widget;
	SDL_Rect rect;
	F_Widget_Draw draw;

	// draw one by one
	while (child_list) {
		child_widget = (STK_Widget *)child_list->rb;

		if (widget->fixed) {
			if (child_widget->rect.y - widget->rect.y + child_widget->rect.h 
						+ widget->border > widget->rect.h)
				break;
			// here, we consider only fixed width of RadioGroup, don't consider height of it,
			// because that is not sense.
			child_widget->rect.w = widget->rect.w - 2 * widget->border;
			// if RadioGroup can't contain too many item, ignore the rest
			STK_WidgetSetDims(child_widget, &child_widget->rect);				
			child_widget->fixed = 1;
		}
		
		draw = STK_WidgetGetDraw(child_widget);
		if (draw)
			draw(child_widget);

		child_list = child_list->next;
	}
		
	return 0;
}


static void STK_RadioGroupEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata)
{
	STK_RadioGroup *rg = (STK_RadioGroup *)object;
	STK_Widget *widget = (STK_Widget *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	STK_RadioButtonListNode *child_list = rg->rblist_head;
	STK_RadioButtonListNode *child_chosen;
	STK_Widget *child_widget;
	
	int mx, my;
	
	mx = event->motion.x;
	my = event->motion.y;
	
	// if mouse point is in radiogroup
	if (STK_WidgetIsInside(widget, mx, my)) {
		// first, we judge which child radiobutton contains mouse
		while (child_list) {
			child_widget = (STK_Widget *)child_list->rb;
			if (STK_WidgetIsInside(child_widget, mx, my)) {
				// only need to change status, redraw action can be done in draw function
				child_widget->state = 1;
				break;
			}
			child_list = child_list->next;		
		}		
		// record the child radiobutton chosen
		child_chosen = child_list;
		
		// count n choices
		child_list = rg->rblist_head;
		rg->nchoice = 0;
		while (child_list) {
			if (child_list->rb->state == STK_RADIOBUTTON_TOGGLE)
				rg->nchoice++;
			child_list = child_list->next;		
		}		

		// Mutex case: here we process other radiobutton according if setting is mutex
		if (rg->mode && rg->nchoice >= 1) {
			child_list = rg->rblist_head;
			// all item clean
			while (child_list) {
				if (child_list != child_chosen && child_list->rb->state == STK_RADIOBUTTON_TOGGLE) {
					child_widget = (STK_Widget *)child_list->rb;
					child_widget->state = 1;
				}
				child_list = child_list->next;		
			}		
			rg->nchoice = 1;
		
		}	
	}
	
	return;
}

int STK_RadioGroupSetMode(STK_RadioGroup *rg, Uint32 m)
{
	if (!rg)
		return -1;
			
	rg->mode = m;
}

