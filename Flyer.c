/*
 | Flyer project
 |
 | Main routines
 |
 | (c) Copywrite 1990   David C. Williams
 |
*/

#include <MacHeaders>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Flyerdef.h"
#include "BATdef.h"


WindowRecord	wStorage;				/* Storage for window record */

MenuHandle 		menus[5];


FlyerViewType	flyerView;

Rect	copyRect;

BATrecord		mainBat;
BATfigure		figures[1+GAME_NMIS+GAME_NFOE];
BATframe		flyerFigs[32],
				boxs[4],
				meanies[8],
				mines[2],
				meanMines[5],
				misFigs[4],
				dieFigs[4],
				boomFigs[8],
				blankFig = {8,8,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

double			sines[32],
				coses[32];

static double	randMax = RAND_MAX;

main() {

	/* Initialize all the system stuff */

	InitGraf( &thePort );
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs( 0L );
	
	/* Initialize sine and cosine table */
	
	FindSines();
	
	/* Make our menus */

	MakeMenus();
	
	/* Initialize bitmaps */
	
	LoadBitMaps();
	
	/* Show about */
	
	InitCursor();	
	ShowAbout();
	
	/* Make our window */

	MakeGameWindow( &flyerView );

	/* Initialize FlyerView */
	
	InitFlyer( &flyerView );						
	flyerView.level = 0;
	
	/* Draw score */
	
	MakeScore( &flyerView );
	
	/* Install VBL animation routine */
	
	StartBAT( &flyerView );
	
	/* Make first window */
	
	UpdateGame( &flyerView );
	
	/* Wait for event */

	EventLoop( &flyerView ); 
	
	/* Finish up */
		
	CloseWindow( flyerView.window );
	
	ExitToShell();
}


int	FindSines()
/*
 | Initialize sine and cosine table 
*/
{
	int		angle;

	for(angle=0;angle<32;angle++) {
		sines[angle] = sin((double)angle*PI_16);
		coses[angle] = cos((double)angle*PI_16);
	}
	
	return;
}
	

int InitFlyerPosition( flyerView )
	FlyerViewType	*flyerView;
/*
 | Initialize position and speed of flyer.  This routine should be called once
 | per flyer life.
*/
{
	flyerView->alive = 1;

	flyerView->angle = 0;
	flyerView->vx = 0.0;
	flyerView->vy = 0.0;
	flyerView->vtot = 0.0;
	flyerView->x = flyerView->winWidth/2.0;
	flyerView->y = flyerView->winHeight/2.0;
	
	return;
}


int InitFlyer( flyerView )
	FlyerViewType	*flyerView;
/*
 | Initialize the "flyerView" record.  This routine should be called at the
 | beginning of each game.
*/
{
	int		iFoe, iMis;

	/* Initialize score and lives */

	flyerView->nlife = GAME_LIVES;
	flyerView->score = 0;
	
	flyerView->paused = 1;
	flyerView->alive = 1;

	/* Initialize flyer position */
	
	InitFlyerPosition( flyerView );
	
	/* Set foes to rewaken in sequence */

	for(iFoe=0;iFoe<GAME_NFOE;iFoe++) 
		flyerView->foes[iFoe].rewake = 30*iFoe + 30;
		
	/* Disable all missles */
	
	for(iMis=0;iMis<GAME_NMIS;iMis++) {
		flyerView->miss[iMis].awake = 0;
	}
	
	return;
}


LoadBitMaps()
/*
 | Load the bitmaps associated with all animation figures.  For convenience,
 | the bitmaps have been stored in an existing type of resource, the cursor.
 | The mask portion of the cursor resource is not used.
*/
{
	CursHandle	aCur;
	int		curs,
			i,
			*bitWord;
			
	/* Load flyer frames */
			
	for(curs=0;curs<32;curs++) {
	
		/* Load the cursor corresponding to the bitmap */
		
		aCur = GetCursor( curs+200 );
		
		flyerFigs[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			flyerFigs[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}
	
	/* Load box frames */
	
	for(curs=0;curs<4;curs++) {
	
		aCur = GetCursor( curs+250 );
		
		boxs[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			boxs[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}
	
	/* Load meanie */
	
	for(curs=0;curs<7;curs++) {
	
		aCur = GetCursor( curs+500 );
		
		meanies[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			meanies[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}
	
	/* Load dying box frames */
	
	for(curs=0;curs<4;curs++) {
	
		aCur = GetCursor( curs+300 );
		
		dieFigs[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			dieFigs[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}
	
	/* Load mines */
	
	for(curs=0;curs<2;curs++) {
	
		aCur = GetCursor( curs+600 );
		
		mines[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			mines[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}	
	
	/* Load mean mines */
	
	for(curs=0;curs<5;curs++) {
	
		aCur = GetCursor( curs+650 );
		
		meanMines[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			meanMines[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}	
	
	/* Load missle figures */
	
	for(curs=0;curs<4;curs++) {
	
		aCur = GetCursor( curs+260 );
		
		misFigs[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			misFigs[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}
	
	/* Load boom figures */
	
	for(curs=0;curs<4;curs++) {
	
		aCur = GetCursor( curs+400 );
		
		boomFigs[curs].hotSpot = (*aCur)->hotSpot;
		for(i=0,bitWord=(int *)&(*aCur)->data;i<16;i++,bitWord++) 
			boomFigs[curs].bits[i] = *bitWord;
			
		ReleaseResource( aCur );
	}		
		
	return;
}


int StartBAT( flyerView )
	FlyerViewType	*flyerView;
/*
 | Initialize the BAT records for the game.  In the game, there are a total
 | of 1 + number foes + number missles figures.
*/
{
	CursPtr		oneFlyer;	
	int			v, h, i, iFoe, iMis, iBoom;
	BATfigure	*oneFig;
	
	SetPort( flyerView->window );

	figures[0].active = 1;
	figures[0].period = 0xffff;
	figures[0].numFrames = 1;
	figures[0].currFrame = 0;
	figures[0].frames[0] = &flyerFigs[0];

	for(iFoe=1;iFoe<=GAME_NFOE;iFoe++) figures[iFoe].active = 0;	
	
	for(iMis=0;iMis<GAME_NMIS;iMis++) {
		oneFig = &figures[1+GAME_NFOE+iMis];
		
		oneFig->active = 0;
		oneFig->period = 3;
		oneFig->numFrames = 4;
		oneFig->currFrame = 0;
		for(i=0;i<4;i++)
			oneFig->frames[i] = &misFigs[i];
	}
		
	BATinit( figures, 1+GAME_NFOE+GAME_NMIS, flyerView->window, &mainBat );
	
	return;
}


MakeGameWindow( flyerView )
	FlyerViewType	*flyerView;
/*
 | Make the main window of the game.  The details of the window are stored in
 | the resource MAIN_WIND.  The size of the window, however, is made to almost fit
 | the size of the screen.
*/
{
	GrafPtr		wPort;
	int			height, width;
	
	/* Get screen size from window manager port */
	
	GetWMgrPort( &wPort );
	
	width = wPort->portRect.right - wPort->portRect.left - 8;
	height = wPort->portRect.bottom - wPort->portRect.top - 46;

	flyerView->winWidth = (double)(width - 16);
	flyerView->winHeight = (double)(height - 34);
	
	/* Get the main window from the resource */

	flyerView->window = GetNewWindow( MAIN_WIND, &wStorage, (void *)-1L );
	
	/* Modify its position and size according to the screen size */
	
	SizeWindow( flyerView->window, width, height, 0 ); 
	MoveWindow( flyerView->window, 4, 40, 0 );

	/* Make it the active window and make it visible */
	
	SelectWindow( flyerView->window );
	ShowWindow( flyerView->window );
	
	return;
}
	

MakeMenus()
/*
 | Make the menus for the game.  All the menus are stored as resources.
*/
{
	int				menu;
		
	/* Make Apple menu, add standard DA menu items */
	
	menus[0] = GetMenu( APPLE_MENU );
	AddResMenu( menus[0], 'DRVR' );
	
	/* Add our file menu, and check paused */
	
	menus[1] = GetMenu( FILE_MENU );
	CheckItem( menus[1], FILE_PAUSE, 1 );
	
	/* Add the default edit menu */
	
	menus[2] = GetMenu( EDIT_MENU );
	
	/* Add the Option menu */
	
	menus[3] = GetMenu( OPTION_MENU );
	
	/* And the level menu and check novice */
	
	menus[4] = GetMenu( LEVEL_MENU );
	CheckItem( menus[4], LEVEL_BEGINNER, 1 );
	
	/* Insert the menus in order onto the menu list */
	
	for (menu=0;menu<5;menu++) InsertMenu( menus[menu], 0 );
	
	/* Draw the menus */
	
	DrawMenuBar();
	
	return;
}


EventLoop( flyerView )
	FlyerViewType	*flyerView;
/*
 | Game event loop.  This routine consists of a loop over events.  If no event
 | is encountered and one system tick has passed, the next frame of game animation
 | is drawn.
 |
 | Refer to chapter 8 "Toolbox Event" in "Inside Macintosh, Volume I" for how to
 | use the event manager.
*/ 
{
	EventRecord		doneEvent;
	WindowPtr 		whichWindow;
	int 			whereCode, size;
	Rect     		moveBound;
	GrafPtr			wPort;
	Point			localWhere;
	long			thisTick,lastTick;
	int				key;
	
	/* Start with a clean slate */
							
	FlushEvents( everyEvent, 0 );
	
	/* 
	 | Calculate the move bounds for the window.  We want to keep the at least
	 | the window bar visible
	*/
	
	GetWMgrPort( &wPort );
	moveBound.left = wPort->portRect.left;
	moveBound.top = wPort->portRect.top + 16;
	moveBound.right = wPort->portRect.right - 4;
	moveBound.bottom = wPort->portRect.bottom - 4; 
	
	/* Set key to inactive */
	
	key = 0;
	
	/* Wait for event */
	
	lastTick = TickCount();
	
	for(;;) {
	
		/* Obscure the cursor */
	
		ObscureCursor();

		/* Wait for next update or mouse down event */
		
		while( !GetNextEvent( everyEvent, &doneEvent )) {
		
			/* No event: check system tick count */
		
			if ((thisTick = TickCount()) > lastTick) {
			
				/* Tick count incremented: if not paused, draw next game frame */
			
				lastTick = thisTick;
				if (!flyerView->paused) {
					if (UpdateGame( flyerView )) {
					
						/* Out of lives ... call end of game handler */
						
						if (EndofGame( flyerView )) return;
					}
				}
				
				/* Perform system task every 32 counts (about 1/2 second) */
			
				if (!thisTick&&0x2F) SystemTask();				
			}
		}
		
		switch(doneEvent.what) {
			case mouseDown:	
			
				/* Mouse was pressed - find out where */
				
				whereCode = FindWindow( doneEvent.where, &whichWindow );
				switch (whereCode) {
				 	case inSysWindow:
				 		SystemClick( &doneEvent, whichWindow );
				 		break;
				 	case inDrag:
				 		DragWindow( whichWindow, doneEvent.where, &moveBound );
				 		break;
				 	case inMenuBar:
				  		if (!DoMenu( doneEvent.where, flyerView )) return;  
				 		break;
				 }
		 		break;
		 	case keyDown:
		 		key = doneEvent.message&charCodeMask, flyerView;
		 		if (key==KEY_SPACE) {
		 			TogglePause( flyerView );
		 			break;
		 		}
		 	case autoKey:
		 		if (!flyerView->paused) ApplyKey( &key, flyerView );
		 		break;
		 	case updateEvt:
				BeginUpdate( flyerView->window );
 				BATnext( &mainBat );
 				MakeScore( flyerView );
 				EndUpdate( flyerView->window );	
				break;
			default:;
	 	}
	}
	
	return;
}


int	ApplyKey( charcode, flyerView )
	int				*charcode;
	FlyerViewType	*flyerView;
/*
 | Apply key press (from event loop).  If the key press is illegal, make
 | a system "beep".
*/
{
	int		hangleOld,
			keycode,
			iMis;
			
	/* Can't do anything if the flyer is dying (exploding) */
			
	if (flyerView->dying) return;
			
	switch (*charcode) {
		case 0: break;
		case KEY_TURNRIGHT:
			flyerView->angle--;
			if (flyerView->angle<0) flyerView->angle = 31;
			break;
		case KEY_TURNLEFT:
			flyerView->angle++;
			if (flyerView->angle==32) flyerView->angle = 0;
			break;
		case KEY_THRUST:
			if (flyerView->vtot < GAME_MAXSPEED) {
				flyerView->vx -= GAME_BURST*sines[flyerView->angle];
				flyerView->vy += GAME_BURST*coses[flyerView->angle];
				flyerView->vtot = sqrt( flyerView->vx*flyerView->vx +
									    flyerView->vy*flyerView->vy   );
			}
			break;
		case KEY_FIRE:
		
			/* Search for a free missle to activate */
			
			for(iMis=0;iMis<GAME_NMIS;iMis++) {
				if (flyerView->miss[iMis].awake==0) {
					flyerView->miss[iMis].awake = GAME_MLIFE;
					flyerView->miss[iMis].x = flyerView->x;
					flyerView->miss[iMis].y = flyerView->y;
					flyerView->miss[iMis].vx =
						flyerView->vx - GAME_MSPEED*sines[flyerView->angle];
					flyerView->miss[iMis].vy =
						flyerView->vy + GAME_MSPEED*coses[flyerView->angle];
				
					/* Disable key repeat for fire */
					
					*charcode = 0;
					
					break;
				}
			}
		
			break;
		default:
		
			/* 
			 | Illegal key press.  This is also a good place to put a break
			 | for debugging.
			*/
		
			SysBeep( 30 );
	}
	
	return;
}


void RamFoe( flyerView, oneFoe, angV, speed, dx, dy )
	FlyerViewType	*flyerView;
	FoeType			*oneFoe;
	double	angV, speed, dx, dy;
/*
 | Update foe angle to point toward current position of flyer.  For
 | 'ordinary' foes.
*/
{	
	double	deltaAngle;

	if (!flyerView->dying) {		
		deltaAngle = atan2( dy, dx ) - oneFoe->angle;
		if (deltaAngle>PI) 
			deltaAngle -= PI2;
		else if (deltaAngle<-PI)
		 	deltaAngle += PI2;
		                    
		if ( (deltaAngle < angV) && (deltaAngle > angV) )
			oneFoe->angle += deltaAngle;
		else if (deltaAngle<0)
			oneFoe->angle -= angV;
		else
			oneFoe->angle += angV;
			
		if (oneFoe->angle>PI) 
			oneFoe->angle -= PI2;
		else if (oneFoe->angle<-PI)
		 	oneFoe->angle += PI2;
	}
		
	/* Update foe's position */
	
	oneFoe->x += speed*cos(oneFoe->angle);
	oneFoe->y += speed*sin(oneFoe->angle);
	
	return;
}


void BlockFoe( flyerView, oneFoe, speed, dx, dy )
	FlyerViewType	*flyerView;
	FoeType			*oneFoe;
	double			speed, dx, dy;
/*
 | Move position of 'mine' foe to block current path of flyer.  The velocity
 | of the foe has a minimum when distant from the path.  As the foe gets closer
 | to the path, its velocity increases as 1/distance.  If the new velocity will
 | make the foe cross the path, its position is placed on the path.
*/
{
	double	cross, v, fsin, fcos;
	
	/* If the flyer is not moving, there is nothing to do! */
	
	if (flyerView->vtot<1E-10) return;
	
	/* Calculate the cross product to get the distance away from the path */
	
	fsin = flyerView->vy/flyerView->vtot;
	fcos = flyerView->vx/flyerView->vtot;	
	
	cross = fcos*dy - fsin*dx;
	
	/* Only move if the foe is off the current path (i.e. non-zero cross product) */
	
	if (cross > 2.0) {
		v = 20.0*speed/cross;
		if (v>cross) 
			v = cross;
		else if (v<speed)
			v = speed;
		oneFoe->x -= v*fsin;
		oneFoe->y += v*fcos;
	}
	else if (cross < -2.0) {	
		v = 20.0*speed/cross;
		if (v<cross) 
			v = cross;
		else if (v>-speed)
			v = -speed;
		oneFoe->x -= v*fsin;
		oneFoe->y += v*fcos;
	}

	return;
}			



int UpdateGame( flyerView  )
	FlyerViewType	*flyerView;
/*
 | Draw the next animation sequence of the game.  This includes calculating the
 | new position of the flyer, the missles and its foes and checking for collisions
 | and deaths.  A death of a foe will increase the score.  A death of a flyer will
 | decrement the number of lives.
 |
 | The order of these operations is probably not important.
 |
 | Returns TRUE (non-zero) if the last flyer life is used up.
*/
{
	int			iFoe, iMis, i, iBoom;
	FoeType		*oneFoe;
	double		deltaAngle;
	BATfigure	*oneFig;
	MisType		*oneMis;
	double		dx, dy,
				prob2;
	int			collision;
	
	double		drag[3] 		= GAME_DRAG,
				foeSpeed[2][3] 	= GAME_FSPEED,
				foeAngV[2] 		= GAME_FANGV,
				foeMspeed[2][3] = GAME_FMSPD;
	int			foeSleep[3] 	= GAME_FSLEEP,
				foeValue[4][3] 	= GAME_FVALUE,
				level;
				
	level = flyerView->level;
	
	/* Update flyer BAT figure */
	
	if (flyerView->dying>0) {
	
		/* Flyer is dying.  Is it time to re-encarnate? */
	
		flyerView->dying--;
		if (flyerView->dying==0) {
		
			/* Our we out of lives ? If so, return TRUE */
			
			if (flyerView->nlife==0) return(1);
	
			/* Time to rewaken */
			
			InitFlyerPosition( flyerView );

			figures->active 	= 1;
			figures->numFrames 	= 1;
			figures->currFrame 	= 0;
			figures->position.h = flyerView->x + 8;
			figures->position.v = flyerView->winHeight - flyerView->y + 26;
			figures->frames[0] 	= &flyerFigs[flyerView->angle];
		}
	}
	else {
	
		/* Use flyer BAT figure corresponding to current angular orientation */
	
		figures->frames[0] = &flyerFigs[flyerView->angle];
		flyerView->alive++;
	}
	
 	/* Keep moving, alive or dead */

	flyerView->x += flyerView->vx;
	flyerView->y += flyerView->vy;
	
	flyerView->vx = drag[level]*flyerView->vx;
	flyerView->vy = drag[level]*flyerView->vy;
	flyerView->vtot = drag[level]*flyerView->vtot;
	
	KEEP_IN_WINDOW( flyerView, flyerView->winWidth, flyerView->winHeight )

	figures->position.h = flyerView->x + 8;
	figures->position.v = flyerView->winHeight - flyerView->y + 26;
	
	/* Care over missles */
	
	for (iMis=0,oneMis=flyerView->miss;iMis<GAME_NMIS;iMis++,oneMis++) {
		oneFig = figures+1+GAME_NFOE+iMis;
	
		/* 
		 | Check awake and activate or deactivate as necessary.  The variable
		 | "awake" is given a positive value when the missle is created to indicate
		 | the lifetime of the missle and is decremented each animation frame.
		 | When the variable reaches zero, the bullet is deactivated.
		*/
		
		if (oneMis->awake > 0) {
			oneFig->active = 1;
			oneMis->awake--;
			
			/* Update missle position */
			
			oneMis->x += oneMis->vx;
			oneMis->y += oneMis->vy;
			
			KEEP_IN_WINDOW( oneMis, flyerView->winWidth, flyerView->winHeight )

			oneFig->position.h = oneMis->x + 8;
			oneFig->position.v = flyerView->winHeight- oneMis->y + 26;
		}
		else
			oneFig->active = 0;
	}

	/* Care over Foes */
	
	collision = 0;

	for (iFoe=0,oneFoe=flyerView->foes;iFoe<GAME_NFOE;iFoe++,oneFoe++) {
		oneFig = &figures[1+iFoe];
			
		/* If foe asleep, decrement rewake counter */
		
		if (oneFoe->rewake > 0) {
			oneFoe->rewake--;
			if (oneFoe->rewake==0) {
			
				/* Time to awaken: which type of foe ? */
				
				prob2 = flyerView->alive;
				prob2 = prob2/(7200.0 + prob2);
				if (prob2>((double)rand())/randMax) 
					oneFoe->type = 1;
				else
					oneFoe->type = 0;
					
				/* Give 1/3 probability of mine foe */
				
				if (rand()/randMax > 0.66667) oneFoe->type += 2;
 				
				/* load and configure figure */
		
				oneFig->active = 1;
				switch(oneFoe->type) {
					case 0:					/* Original foe */
						oneFig->period = 2;
						oneFig->numFrames = 4;
						oneFig->currFrame = iFoe;   
						for(i=0;i<4;i++)
							oneFig->frames[i] = &boxs[i];
						break;
					case 1:					/* Nasty foe */
						oneFig->period = 2;
						oneFig->numFrames = 7;
						oneFig->currFrame = iFoe;   
						for(i=0;i<7;i++)
							oneFig->frames[i] = &meanies[i];
						break;
					case 2:					/* Simple mine */
						oneFig->period = 20;
						oneFig->numFrames = 2;
						oneFig->currFrame = iFoe&1;
						oneFig->frames[0] = &mines[0];
						oneFig->frames[1] = &mines[1];
						break;
					case 3:					/* Mean mine */
						oneFig->period = 10;
						oneFig->numFrames = 4;
						oneFig->currFrame = iFoe&3;
						for(i=0;i<4;i++)
							oneFig->frames[i] = &meanMines[i];
						break;
				}
									
				/* 
				 | Pick random location for foe's appearance, making sure that
				 | it isn't unfairly close to the flyer 
				*/
				
				oneFoe->dying = 0;
				oneFoe->angle = PI2*((double)rand())/randMax;
				
				do {
					oneFoe->x = flyerView->winWidth*((double)rand())/randMax;
					oneFoe->y = flyerView->winHeight*((double)rand())/randMax;
				} while ( (flyerView->x+80 > oneFoe->x ) &&
				          (flyerView->x-80 < oneFoe->x ) &&
				          (flyerView->y+80 > oneFoe->y ) &&
				          (flyerView->y-80 < oneFoe->y )    );
			}
			else
				continue;
		} 
		
		/* Check for collision with Flyer */

		dx = flyerView->x - oneFoe->x;
		dy = flyerView->y - oneFoe->y;

		if (!flyerView->dying) {			
			if ( dx>-8 && dx<8 && dy>-8 && dy<8 ) collision = 1;
		}
		
		if (oneFoe->type<2) 
		
			/* Get ordinary foe to turn into current position of flyer */
		
			RamFoe( flyerView, oneFoe, foeAngV[oneFoe->type] + 0.02*iFoe, 
			        foeSpeed[oneFoe->type][level], dx, dy     );
		else 
		
			/* Get mine to move into path of flyer */
			
			BlockFoe( flyerView, oneFoe, foeMspeed[oneFoe->type-2][level], dx, dy );
			
		/* Keep foe position inside the window */	
					
		KEEP_IN_WINDOW( oneFoe, flyerView->winWidth, flyerView->winHeight )
				
		figures[1+iFoe].position.h = oneFoe->x + 8;
		figures[1+iFoe].position.v = flyerView->winHeight - oneFoe->y + 26;
		
		/* 
		 | Check to see if this Foe is done exploding, else if it is not
		 | exploding, check to see if it has been intercepted by a missle.
		*/
					
		if (oneFoe->dying && oneFig->currFrame==4) {
			oneFoe->rewake = foeSleep[level];
			oneFoe->dying = 0;
		} 
		else if (!oneFoe->dying) {
		
		 	/* Check for missle's interception */
		
			for (iMis=0,oneMis=flyerView->miss;iMis<GAME_NMIS;iMis++,oneMis++) {
			
				if (oneMis->awake) {
			
					if ( (oneMis->x+8 > oneFoe->x) &&
					     (oneMis->x-8 < oneFoe->x) &&
					     (oneMis->y+8 > oneFoe->y) &&
					     (oneMis->y-8 < oneFoe->y)    ) {
					     
						/* Missle caught !! substitute dying frames */
						
						oneFoe->dying = 1;
						
						oneFig->numFrames = 4;
						oneFig->currFrame = 0;
						oneFig->active = 2;
						oneFig->period = 2;
						for(i=0;i<4;i++) oneFig->frames[i] = &dieFigs[i];
						
						/* Kill the missle */
						
						oneMis->awake = 0;
						
						/* Add to score */
						
						flyerView->score += foeValue[oneFoe->type][level];
						UpdateScore( flyerView );
						
						break;
					}
				}
			}
		}		
		
		
	}
	
	/* At this point, check for the collision of a foe with the flyer */
	
	if (collision && !flyerView->dying) {
	
		/* 
		 | Collision detected: change flyer animation to explosion and
		 | set dying counter.
		*/
		
		flyerView->dying = 120;
		
		for(iBoom=0;iBoom<8;iBoom++) 
			figures->frames[iBoom] = &boomFigs[iBoom];

		figures->active = 2;			
		figures->period = 5;
		figures->currFrame = 0;
		figures->numFrames = 8; 
		
		flyerView->nlife--;
		UpdateLife( flyerView ); 
	}
		
	/* Call BAT to draw the new animation frame */
	
	BATnext( &mainBat );
	
	return( 0 );
}


int UpdateScore( flyerView )
	FlyerViewType	*flyerView;
/*
 | Update the score to the current value.
*/
{
	char 	score[40];
	Rect	scoreErase = { 0, 65, 20, 110 };
	
	SetPort( flyerView->window );
	
	sprintf( score, "%ld", flyerView->score );
	CtoPstr( score );
	MoveTo( 65, 12 );
	
	EraseRect( &scoreErase );
	DrawString( score );
	
	return;
}

int UpdateLife( flyerView )
	FlyerViewType	*flyerView;
/*
 | Update the number of flyer lives to the current value.
*/
{
	int		life;
	Rect	putLife,
			srcLife = {0,0,16,16};
	BitMap	lifeMap = {(char *)flyerFigs->bits,2,{0,0,16,16}};

	SetPort( flyerView->window );
	
	putLife.top = 2;
	putLife.bottom = 18;
	for(life=0;life<GAME_LIVES;life++) {
		putLife.left = life*20 + 320;
		putLife.right = putLife.left + 16;
		
		if (life<flyerView->nlife)
			CopyBits( &lifeMap, &flyerView->window->portBits, &srcLife,
		          	  &putLife, srcCopy, 0 );
		else
			EraseRect( &putLife );
			
		FrameRect( &putLife );		          
	}
	
	return;
}


int UpdatePauseMes( flyerView )
	FlyerViewType	*flyerView;
/*
 | Update the pause message to indicate current pause state
*/
{
	Rect	eraseRect = { 0, 150, 20, 310 };

	SetPort( flyerView->window );
	
	EraseRect( &eraseRect );
	
	MoveTo( 150,12 );
	if (flyerView->paused)
		DrawString( "\pPress <space> to continue" );
	else
		DrawString( "\pPress <space> to pause" );
		
	return;
}
		


int	MakeScore( flyerView )
	FlyerViewType	*flyerView;
/*
 | Make score.  This routine forms all the initial text of the score and
 | should be called when the game window is made or updated.  To set a new
 | value for the score, call UpdateScore.  To change the number of lives, call
 | UpdateLife
*/
{
	int		life;
	Rect	putLife;
	Rect	scoreSpace = {0,0,20,500};

	SetPort( flyerView->window );
	
	EraseRect( &scoreSpace );
	
	MoveTo( 20, 12 );
	DrawString( "\pSCORE:");
	UpdatePauseMes( flyerView );
	
	UpdateScore( flyerView );
	UpdateLife( flyerView );
	
	return;
}
		

int	DoMenu( where, flyerView )
	Point			where;
	FlyerViewType	*flyerView;
/*
 | Perform menu action.  This routine is called by EventLoop for menu events.
*/
{
	union {
		short		byWord[2];
		long		byLong;
	} menuId;
	
	char			appleName[255];
	
	/* Get menu selection */
	
	menuId.byLong = MenuSelect( where );
	
	if (menuId.byWord[1]!=0) {
	
	 	/* Valid menu selection */
	 	
	 	switch(menuId.byWord[0]) {
	 	
	 		case APPLE_MENU:				
	 			switch(menuId.byWord[1]) {
	 				case APPLE_ABOUT:
	 				
	 					/* "About Flyer" selected: show about window */
	 				
	 					ShowAbout();
		 				break;
		 			default:
		 			
		 				/* Perform DA selection */
		 			
		 				GetItem( menus[0], menuId.byWord[1], appleName );
		 				OpenDeskAcc( appleName );
		 		}
		 		break;
			case FILE_MENU:					/* In File Menu */
	 			switch(menuId.byWord[1]) {
	 				case FILE_QUIT:
	 				
	 					/* Quit selected: stop the game */
	 				
	 					HiliteMenu(0);
	 					return(FALSE);
	 					break;
	 				case FILE_PAUSE:
	 				
	 					/* Pause selected: toggle the pause state */
	 				
	 				    TogglePause( flyerView );
	 					break;
	 			}
	 			break;
	 		case OPTION_MENU:
	 			switch(menuId.byWord[1]) {
	 				case OPTION_CLRTT:
	 					ClearTopTen();
	 					break;
	 			}
	 			break;
	 		case LEVEL_MENU:
	 			if (menuId.byWord[1] != flyerView->level+1) {
	 				CheckItem( menus[4], flyerView->level+1, 0 );
	 				CheckItem( menus[4], menuId.byWord[1], 1 );
	 				flyerView->level = menuId.byWord[1] - 1;
	 			}
	 			break;
	 	}
	}
	
	HiliteMenu(0);
	
	return(TRUE);
}


int TogglePause( flyerView )
	FlyerViewType	*flyerView;
/* 
 | Toggle pause state
*/
{
	flyerView->paused = !flyerView->paused;

	CheckItem( menus[1], FILE_PAUSE, flyerView->paused );
	UpdatePauseMes( flyerView );

	return;
}



int		ClearTopTen()
/*
 | Clear the topten scorers
*/
{
	TopTenHndl	topsHndl;
	OneTopTen	*theTops;
	int			i;
	
	/* Confirm the command */
	
	if (CautionAlert( CLEAR_ALRT, 0 )==1) return;
	
	/* Get "scrs" resource */
	
	topsHndl = (TopTenHndl)GetResource( 'scrs', 128 );
	
	/* Set all scores to zero */
	
	theTops = **topsHndl;
	for(i=0;i<10;i++) {
		theTops[i].inits[0] = 0;
		theTops[i].score = 0;
	}
	
	/* Indicate a change in this resource */
	
	ChangedResource( topsHndl );			

	/* Return scrs resource */
	
	ReleaseResource( topsHndl );
	
	return;
}



int		EndofGame( flyerView )
	FlyerViewType	*flyerView;
/*
 | End of game handler:  If the score is better than the last top ten, ask for
 | player initials and include the new score in the top ten.  Then ask for
 | a new game.
 |
 | 'scrs' is a custom resource designed for this game.
*/
{
	TopTenHndl	topsHndl;
	int			i, j;
	
	int		done;
	
	/* Get "scrs" resource */
	
	topsHndl = (TopTenHndl)GetResource( 'scrs', 128 );
	
	/* Check top ten against current score */
	
	CheckTopTen( flyerView, topsHndl );
	
	done = ShowTopTen( topsHndl );
	
	/* Return scrs resource */
	
	ReleaseResource( topsHndl );
	
	if (!done) {
	
		/* Start new game: clear window */
		
		SetPort( flyerView->window );
		EraseRect( &flyerView->window->portRect );
		
		/* Initialize flyerView */
		
		InitFlyer( flyerView );						
		
		/* Reinstall VBL animation routine */
	
		StartBAT( flyerView );
		
		/* Make score */
		
		MakeScore( flyerView );
	}
	
	return(done);
}


int	CheckTopTen( flyerView, topsHndl )
	FlyerViewType	*flyerView;
	TopTenHndl			topsHndl;
{
	int			i, j;
	OneTopTen	*theTops = **topsHndl;
		
	if (flyerView->score > theTops[9].score) {
	
		/* Current score inside top ten: find where */
		
		i = 9;
	    while( flyerView->score > theTops[i-1].score && i>0 ) i--;
	    
	    /* Move current scores down */
	    
	    for(j=9;j>i;j--) theTops[j] = theTops[j-1];
	    
	    /* Put in new score */
	    
	    GetInitials( theTops[i].inits );
	    theTops[i].score = flyerView->score;
	    
	    /* Indicate that this resource has changed */
	    
	    ChangedResource( topsHndl );		
	}
	
	return;
}


Rect	defButRect;

pascal void OutlineDefBut( theWindow, itemNo )
	WindowPtr	theWindow;
	int			itemNo;
/*
 | User procedure for the useritem in the Set Coord dialog that outlines
 | the 'OKAY' button.
*/
{
	SetPort( theWindow ); 

	PenSize(3,3);
	InsetRect( &defButRect, -4, -4 );
	FrameRoundRect( &defButRect, 16, 16);
	
	return;
}



int		GetInitials( initials )
	char		initials[3];
{
	DialogPtr	initsDial;
	int			itemType,
				itemHit;
	Rect		box;
	char		initStr[255];
	ControlHandle	okay,
					inits,
					ngOutline;

	/* Make our dialog window */

	initsDial = GetNewDialog( INITS_DIAL, 0L, (void *)-1L );
	
	/* Put Okay's rectangle into global variables for it's user-item */

	GetDItem( initsDial, IN_OKAY, &itemType, &okay, &defButRect );	
	
	/* Set the outline user item procedure */
	
	GetDItem( initsDial, IN_OL_OKAY, &itemType, &ngOutline, &box );	
	SetDItem( initsDial, IN_OL_OKAY, userItem, OutlineDefBut, &box );
	
	/* Get initial control and select all text */
	
	GetDItem( initsDial, IN_INITS, &itemType, &inits, &box );
	SelIText( initsDial, IN_INITS, 0, 32767 );
		
	/* Our dialog window is complete: make it appear */
	
	ShowWindow( initsDial );
	
	/* Call model dialog handler */

	do {
		ModalDialog( 0L, &itemHit );
	} while ( itemHit!=IN_OKAY );
	
	/* Get the initials string */
	
	GetIText( inits, initStr );
	PtoCstr( initStr );
	if (strlen( initStr ) < 3) strcat( initStr, "   " );
	initials[0] = initStr[0];
	initials[1] = initStr[1];
	initials[2] = initStr[2];
		
	/* We're done: dispose of dialog window */
	
	DisposDialog( initsDial );
	
	return;
}


int		ShowTopTen( topsHndl )
	TopTenHndl	topsHndl;
{
	OneTopTen	*theTops;
	DialogPtr	toptenDial;
	int			itemType,
				itemHit,
				line;
	char		aline[255];
	Rect		box;
	ControlHandle	newgame,
					oneLine,
					ngOutline;

	/* Make our dialog window */

	toptenDial = GetNewDialog( TOPTEN_DIAL, 0L, (void *)-1L );
	
	/* Put New Game's rectangle into global variables for it's user-item */

	GetDItem( toptenDial, TT_NEWGAME, &itemType, &newgame, &defButRect );	
	
	/* Set the outline user item procedure */
	
	GetDItem( toptenDial, TT_OL_NEWGAME, &itemType, &ngOutline, &box );	
	SetDItem( toptenDial, TT_OL_NEWGAME, userItem, OutlineDefBut, &box );
	
	/* Write out the top 10 */
	
	theTops = **topsHndl;
	for (line=0;line<10;line++) {
		sprintf( aline, "%3s", theTops[line].inits );
		CtoPstr( aline );
		GetDItem( toptenDial, TT_FIRSTTOP+line, &itemType, &oneLine, &box );
		SetIText( oneLine, aline );
		
		sprintf( aline, "%ld", theTops[line].score );
		CtoPstr( aline );
		GetDItem( toptenDial, TT_FIRSTSCR+line, &itemType, &oneLine, &box );
		SetIText( oneLine, aline );
	}
			
	/* Our dialog window is complete: make it appear */
	
	ShowWindow( toptenDial );
	
	/* Call model dialog handler until 'Quit' or 'New Game' hit */

	ModalDialog( 0L, &itemHit );
		
	/* We're done: dispose of dialog window */
	
	DisposDialog( toptenDial );
	
	return( itemHit==TT_QUIT );
}
