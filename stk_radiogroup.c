#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_base.h"
#include "stk_widget.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"

#define STK_RADIOGROUP_BORDER_THICKNESS	2



static void STK_RadioGroupEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata);


//
// char *radiostr[] = {"A", "AB", "ABC", "ABCD", "ABDEFGFFDSF"};
//
STK_Widget *STK_RadioGroupNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *radiostr[], int num)
{	
	STK_RadioGroup *rg;
	STK_RadioButton *rb;
	STK_RadioButtonListNode *rbln, *ahead;
	STK_Widget *widget;
	
	int i = 0;
	
	rg = (STK_RadioGroup *)STK_Malloc(sizeof(STK_RadioGroup));
	widget = (STK_Widget *)rg;
	
	widget->type = STK_WIDGET_RADIOGROUP;
	widget->name = "RadioGroup";
	widget->flags |= WIDGET_FOCUSABLE;

	widget->rect.x = x;
	widget->rect.y = y;	
	widget->rect.w = w;
	widget->rect.h = h;
	
	rg->n = num;
	rg->name = radiostr;
	rg->border = STK_RADIOGROUP_BORDER_THICKNESS;
	rg->interval = 2 * rg->border;
	
	rg->nchoice = 0;
	rg->mono = 0;
	rg->fixed = 0;
	rg->border_on = 0;

	STK_SignalConnect(widget, "mousebuttondown", STK_RadioGroupEventMouseButtonDown, widget);
	
	// create sub widgets of radio group: single list
	for (i=0; i<num; i++) {
		// create sub widget list node
		ahead = (STK_RadioButtonListNode *)STK_Malloc(sizeof(STK_RadioButtonListNode));
		
		// here, we think the height of the font is 30, it must be a global variable
		// here, we know, the coordination of those child radio buttons are relative to window (absoluted)
		// , but not radiogroup right now, may be we need have a filter procedure to change this
		rb = (STK_RadioButton *)STK_RadioButtonNew(
						radiostr[i], 
						x + rg->border, 
						y + rg->border + i*(30 + rg->interval),
						100,
						30 );
		// put radio button object into each list node
		ahead->rb = rb;
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
	
	return widget;
}

void STK_RadioGroupDraw(STK_Widget *widget)
{
	STK_RadioGroup *rg = (STK_RadioGroup *)widget;
	STK_RadioButtonListNode *child_list = rg->rblist_head;
	STK_Widget *child_widget;
	
	SDL_Rect rect, rect_B;
	int i;
	Uint16 width, height;
	
	// if have nothing to draw
	if (!rg->rblist_head)
		return;
	
	child_widget = rg->rblist_head->rb;
	// we must calculate the max length of child radio buttons
	width = child_widget->rect.w;
	while (child_list) {
		// if child_list if true, we consider child_list->rb is also true
		child_widget = child_list->rb;
		if (child_widget->rect.w > width)
			width = child_widget->rect.w;
		
		child_list = child_list->next;
	}
	// scale all radiobutton's width to this maxmun width
	child_list = rg->rblist_head;
	while (child_list) {
		// if child_list if true, we consider child_list->rb is also true
		child_widget = (STK_Widget *)child_list->rb;
		child_widget->rect.w = width;
		
		child_list = child_list->next;
	}
	
	// calculate height
	height = rg->border*2 + 30*rg->n + rg->interval*(rg->n - 1);
	
	// set adapter reference
	rect_B.x = widget->rect.x;
	rect_B.y = widget->rect.y;
	rect_B.w = width;
	rect_B.h = height;		

	// backup
	STK_BaseRectCopy(&rect, &widget->rect);
	// before drawing, we need to do some computation.
	STK_BaseRectAdapter(&widget->rect, &rect_B);
        // if area has changed, need to reset dimensions of this label: free previous surface and alloc a new one
        if ( !STK_BaseRectEqual(&widget->rect, &rect) )
                STK_WidgetSetDims(widget, &widget->rect);
	
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
	
	printf("Enter STK_RadioGroupFilling...\n");
	printf("n == %d\n", rg->n);
	// draw one by one
	while (child_list) {
		child_widget = (STK_Widget *)child_list->rb;
		
		draw = STK_WidgetGetDraw(child_widget);
		if (draw)
			draw(child_widget);
	
		// calculate the relative coordination
		rect.x = child_widget->rect.x - widget->rect.x;
		rect.y = child_widget->rect.y - widget->rect.y;
		rect.w = widget->rect.w;
		rect.h = widget->rect.h;

		printf("Ready to blit...\n");
		// blit label's surface to radio button's surface
		SDL_BlitSurface(child_widget->surface, NULL, widget->surface, &rect);

		child_list = child_list->next;
	}
	printf("Exit STK_RadioGroupFilling...\n");
	
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
				child_widget->state = 1;
				STK_WidgetEventRedraw(child_widget);
				break;			
			}
			child_list = child_list->next;		
		}		
		// record the child radiobutton chosen
		child_chosen = child_list;
		
		// Mutex case: here we process other radiobutton according if setting is mutex
		if (rg->mono && rg->nchoice >= 1) {
			child_list = rg->rblist_head;
			// all item clean
			while (child_list && child_list != child_chosen) {
				// if the radiobutton is chosen already, then we imtate mouse button down effect
				if (child_list->rb->state == STK_RADIOBUTTON_TOGGLE) {
					child_widget = (STK_Widget *)child_list->rb;
					// seems mouse button down
					child_widget->state = 1;
					STK_WidgetEventRedraw(child_widget);
				}
				child_list = child_list->next;		
			}		
			rg->nchoice = 1;
		
		}	
	}

}



