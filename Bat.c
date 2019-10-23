/*
 | BAT: Bit Animation tool: facility for small, bit mapped animation
 |
 | Main routines
 |
 | (c) Copyright 6/2/90  David C. Williams   All rights reserved
*/

#include <asm.h>
#include <string.h>
#include <MacHeaders>

#include "Batdef.h"

/* BAT global variables */

static short	BATallzero[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

void BATcopyBits( Point, short*, short*, BitMap*, Rect );

int	BATinit( BATfigure *BatFigs, int numFigs, 
             WindowPtr window, BATrecord *batRecord )
{
	int			fig;
	BATfigure	*thisFig;

	/* Initialize global counter */
	
	batRecord->count = 0;
	
	/* Save parameters */
	
	batRecord->figures = BatFigs;
	batRecord->numFig = numFigs;
	batRecord->window = window;
	
	/* Initialize local counters and saves */

	for(fig=0,thisFig=BatFigs;fig<numFigs;fig++,thisFig++) {
		thisFig->localCount = 0;
		thisFig->save[0].active = 0;
		thisFig->save[1].active = 0;
	}
	
	return;
}

int	BATnext( BATrecord *batRecord )
{
	int				fig, fig2, save;
	int				currSave, lastSave;
	register short	i, *to, *from;
	BATfigure		*thisFig;
	Point			aligned, saveAligned;
	BATframe		*curr;
	BitMap			map     = {0,2,{0,0,16,16}},
					mapSave = {0,2,{0,0,16,16}};
	Rect			srcRect = {0,0,16,16};
	BATsave			*thisSave;
	
	/* Set port */
	
	SetPort( batRecord->window );
	
	/* Increment global pointer */
	
	batRecord->count++;
	
	currSave = batRecord->count&1;
	lastSave = currSave ? 0 : 1;
	
	/* Write loop: */
	
	for(fig=0,thisFig=batRecord->figures;fig<batRecord->numFig;fig++,thisFig++) {
	
		if (!thisFig->active) 
			thisFig->save[lastSave].active = 0;
		else {
		
			/* Figure is active: */
		
			if (thisFig->numFrames>1) {
			
				/* Increment local counter and check for next frame */
				
				thisFig->localCount++;
				if (thisFig->localCount>thisFig->period) {
					thisFig->currFrame++;
					thisFig->localCount = 0;
					if (thisFig->currFrame>=thisFig->numFrames) {
						if (thisFig->active==2) {
							thisFig->active = 0;
							continue;
						}
						thisFig->currFrame = 0;
					}
				}
			}
			curr = thisFig->frames[thisFig->currFrame];
			
			/* Find aligned point */
			
			aligned.h = thisFig->position.h - curr->hotSpot.h;
			aligned.v = thisFig->position.v - curr->hotSpot.v;
			
			/* Copy to screen */
			
			map.baseAddr = (char *)curr->bits;
			BATcopyBits( aligned, curr->bits, curr->bits, 
						 &batRecord->window->portBits,
						 batRecord->window->portRect ); 
						 
			/* Copy to last save */
			
			/*memcpy( thisFig->save[lastSave].bits, curr->bits, 32 );*/
			
			
			to   = thisFig->save[lastSave].bits;
			from = curr->bits;
			do { *(to++) = *(from++); } while( from < curr->bits + 16 );
			
			thisFig->save[lastSave].aligned = aligned;
			thisFig->save[lastSave].active = 1;
			
			/* Loop over all active current saves and mask if close */
			
			for( fig2=0;fig2<batRecord->numFig;fig2++) {
				thisSave = &batRecord->figures[fig2].save[currSave];
				if (thisSave->active) {
				
					/* Build an appropriately aligned destination rectangle */
					
					saveAligned.h = aligned.h - thisSave->aligned.h;
					saveAligned.v = aligned.v - thisSave->aligned.v;
					
					/* Check to see if new frame overlaps old */

					if (saveAligned.h < -15 || saveAligned.h > 15 ||
					    saveAligned.v < -15 || saveAligned.v > 15    ) continue;
					 
					/* Copy bits */
					
					map.baseAddr = (char *)thisSave->bits;
					BATcopyBits( saveAligned, BATallzero, curr->bits,
					             &map, srcRect ); 
					
				}
			}
		}		 /* Active figure */

	}     /* Write loop */
	
	/* Go through lift loop */
	
	for( fig2=0;fig2<batRecord->numFig;fig2++) {
		thisSave = &batRecord->figures[fig2].save[currSave];
		if(thisSave->active) {
		
			BATcopyBits( thisSave->aligned, BATallzero, thisSave->bits, 
			             &batRecord->window->portBits, 
			             batRecord->window->portRect );
			
		}
	} 
	
	return;
}
			 
		
		
void BATcopyBits( srcPoint, srcBase, maskBase, dstBits, dstRect )
	Point	srcPoint;
	short	*srcBase, *maskBase;
	BitMap	*dstBits;
	Rect	dstRect;
{
	short	shiftright,
			shiftleft,
			boundsmask,
			startline2,
			endline2,
			endline;
	Rect	truebounds;
	asm {
	
		;Save registers not saved by Think C

		movem	d3-d4/a2,-(sp)		
			
		;Copy base addresses of dstBit argument

		movea.l	dstBits,a0
		
		;Copy base address of destination bitmap into a1
			
  		movea.l	OFFSET(BitMap,baseAddr)(a0),a1	

		;Get true bounds as the inclusion of dstBits.bounds and dstRect
		
		move.w	dstRect.top,d0
		move.w	OFFSET(BitMap,bounds.top)(a0),d1
		cmp.w	d1,d0
		bge		@top_inbounds
		move.w	d1,truebounds.top
		bra		@check_leftbound
	@top_inbounds:
		move.w	d0,truebounds.top
		
	@check_leftbound:
		move.w	dstRect.left,d0
		move.w	OFFSET(BitMap,bounds.left)(a0),d1
		cmp.w	d1,d0
		bge		@left_inbounds
		move.w	d1,truebounds.left
		bra		@check_botbound
	@left_inbounds:
		move.w	d0,truebounds.left
		
	@check_botbound:
		move.w	dstRect.bottom,d0
		move.w	OFFSET(BitMap,bounds.bottom)(a0),d1
		cmp.w	d1,d0
		ble		@bot_inbounds
		move.w	d1,truebounds.bottom
		bra		@check_rightbound
	@bot_inbounds:
		move.w	d0,truebounds.bottom
		
	@check_rightbound:
		move.w	dstRect.right,d0
		move.w	OFFSET(BitMap,bounds.right)(a0),d1
		cmp.w	d1,d0
		ble		@right_inbound
		move.w	d1,truebounds.right
		bra		@find_lines
	@right_inbound:
		move.w	d0,truebounds.right
		
		;Find starting line and ending lines
		
	@find_lines:
		move.w	#0,startline2		;Set defaults
		move.w	#30,endline2
		move.w	#15,endline
		
		move.w	truebounds.top,d0
		sub.w	srcPoint.v,d0
  		ble		@top_in				;Check out-of-bounds top line
  		cmp.w	#15,d0
  		bgt		@outside
  		asl.w	#1,d0
  		move.w	d0,startline2
  	@top_in:
  		move.w	truebounds.bottom,d0
  		sub.w	srcPoint.v,d0
  		ble		@outside
  		cmp.w	#16,d0
  		bge		@bot_in
 		sub.w	#1,d0
  		move.w	d0,endline
  		asl.w	#1,d0
  		move.w	d0,endline2
 	@bot_in:
  	  	
		;calculate left-right border mask
		
		move.l	#0xffff,d1		;	Set default
		
		move.w	truebounds.left,d0
		sub.w	srcPoint.h,d0
  		ble		@left_in			;Check out-of-bounds left edge
  		cmp.w	#15,d0
  		bgt		@outside
  		lsr.w	d0,d1
  		bra		@right_in
  	@left_in:
  		move.w	srcPoint.h,d0
  		add.w	#16,d0
  		sub.w	truebounds.right,d0
  		ble		@right_in			;Check out-of-bounds right edge
  		cmp.w	#15,d0
  		bge		@outside
   		asl.w	d0,d1
  	@right_in:
  		move.w	d1,boundsmask

  		;Calculate address offset for top of figure
  	
  		move.w	srcPoint.v,d0
  		add.w	dstRect.top,d0 		
		sub.w	OFFSET(BitMap,bounds.top)(a0),d0
		add.w	endline,d0
  		ext.l	d0
 		mulu.w	OFFSET(BitMap,rowBytes)(a0),d0
		adda.l	d0,a1
		
		;Offset a1 to horizontal word position and calculate bit shift values
		
		move.w	srcPoint.h,d0
		add.w	dstRect.left,d0
		sub.w	OFFSET(BitMap,bounds.left)(a0),d0
		move.w	d0,d1
		and.w	#0x0007,d1
		move.w	d1,shiftright
		move.w	#16,d1
		sub.w	shiftright,d1
		move.w	d1,shiftleft
		asr.w	#3,d0
		ext.l	d0
		add.l	d0,a1

		;Draw the figure, line by line
		
	@draw_start:
		move.w	OFFSET(BitMap,rowBytes)(a0),d1
		ext.l	d1
		movea.l	srcBase,a0
		movea.l	maskBase,a2
		move.w	endline2,d0
	@drawloop:
		move.w	(a0,d0),d3
		move.w	(a2,d0),d4
		and.w	boundsmask,d4
		beq		@draw_cont
		and.w	d4,d3
		move.w	shiftright,d2
		beq 	@draw_noshift
		movem	d3-d4,-(sp)		
		lsr.w	d2,d3
		lsr.w	d2,d4
		not.w	d4
		and.w	d4,(a1)
		or.w	d3,(a1)
		movem	(sp)+,d3-d4
		move.w	shiftleft,d2
		lsl.w	d2,d3
		lsl.w	d2,d4
		not.w	d4
		and.w	d4,2(a1)
		or.w	d3,2(a1)
		bra		@draw_cont 
	@draw_noshift:
		not.w	d4
		and.w	d4,(a1)
		or.w	d3,(a1)
	@draw_cont:
		suba.l	d1,a1
		subq.w	#2,d0
		cmp.w	startline2,d0
		bge		@drawloop
		
		; Return d3,a2
	
	@outside:
		movem	(sp)+,d3-d4/a2
	}
	return;
}
			
		
		
		
		
		
		