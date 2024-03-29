/*
 | Flyer: About and Help window routines
*/

#include <MacHeaders>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Flyerdef.h"
#include "BATdef.h"

BATrecord		helpBat;
BATfigure		helpFigs[4];

PicHandle		aboutPict=0, helpPict=0;
WindowPtr		aboutWind, helpWind;

extern BATframe		boxs[4],
					meanies[8],
					mines[2],
					meanMines[5];

int	ShowAbout()
/*
 | Display the "About" window.  This window has two buttons: "exit" to continue
 | with the game and "help" for directions.  These controls are handled manually
 | here rather than through the dialog manager so as to have greater control
 | over the window size and contents.
*/
{
	GrafPtr		savePort;
	int			whereCode,
				frame,
				thisTick,
				lastTick,
				xyoff;
	ControlHandle	quitButton,
					helpButton,
					whichButton;
	Rect		pictRect;
	EventRecord	doneEvent;
	WindowPtr	whichWindow;
	
	/* Save current grafport, to be put back later */
	
	GetPort( &savePort );
	
	/* Get the about window and it's picture */
	
	aboutPict = GetPicture( ABOUT_PICT );
	aboutWind = GetNewWindow( ABOUT_WIND, 0, (void *)-1L );
	
	/* Size window to picture plus 20 pixels for buttons */
	
	pictRect = (*aboutPict)->picFrame;
	pictRect.bottom += 24;
	
	SizeWindow( aboutWind, pictRect.right, pictRect.bottom, 0 );
	
	/* Get Quit and Help buttons and place them in the lower corners */
	
	quitButton = GetNewControl( ABOUT_CNTL_QUIT, aboutWind );
	xyoff = pictRect.bottom - (*quitButton)->contrlRect.bottom - 2;
	(*quitButton)->contrlRect.top += xyoff;
	(*quitButton)->contrlRect.bottom += xyoff;
	xyoff = 20 - (*quitButton)->contrlRect.left;
	(*quitButton)->contrlRect.left += xyoff;
	(*quitButton)->contrlRect.right += xyoff;
	
	helpButton = GetNewControl( ABOUT_CNTL_HELP, aboutWind );
	xyoff = pictRect.bottom - (*helpButton)->contrlRect.bottom - 2;
	(*helpButton)->contrlRect.top += xyoff;
	(*helpButton)->contrlRect.bottom += xyoff;
	xyoff = pictRect.right - (*helpButton)->contrlRect.right - 20;
	(*helpButton)->contrlRect.left += xyoff;
	(*helpButton)->contrlRect.right += xyoff;
	
	/* Open window, set grafport, and draw about picture */
	
	ShowWindow( aboutWind );
	DrawControls( aboutWind );
	SelectWindow( aboutWind );
	
	SetPort( aboutWind );
	
	pictRect.bottom -= 24;
	DrawPicture( aboutPict, &pictRect );
	
	/* Wait for a mouse down event inside the window */
	
	FlushEvents( everyEvent, 0 );
	lastTick = TickCount();
		
	for(;;) {
		while(!GetNextEvent( mDownMask|updateMask, &doneEvent ));
		
		if (doneEvent.what==mouseDown) {
			whereCode = FindWindow( doneEvent.where, &whichWindow );
			if (whereCode == inContent && whichWindow == aboutWind ) {
				GlobalToLocal( &doneEvent.where );
				if (FindControl( doneEvent.where, aboutWind, &whichButton )) {
					TrackControl( whichButton, doneEvent.where, 0 );
					break;
				}
				SysBeep( 30 );
			}
			else
				SysBeep( 30 );
		}
		else {
			BeginUpdate( aboutWind );
			DrawPicture( aboutPict, &pictRect );
			EndUpdate( aboutWind );
		}

	}
	
	/* Kill the window and the picture  */

	ReleaseResource( aboutPict );	
	DisposeWindow( aboutWind );
	
	/* If the "Help" button was pressed, draw help window */
	
	if (whichButton == helpButton) ShowHelp();
	
	/* Return the saved port */
	
	SetPort( savePort );
	
	return;
}


int	ShowHelp()
{
	GrafPtr		savePort;
	int			whereCode,
				frame,
				thisTick,
				lastTick;
	Rect		pictRect;
	EventRecord	doneEvent;
	WindowPtr	whichWindow;
	
	/* Save current grafport, to be put back later */
	
	GetPort( &savePort );
	
	/* Get the about window and it's picture */
	
	helpPict = GetPicture( HELP_PICT );
	helpWind = GetNewWindow( HELP_WIND, 0, (void *)-1L );
	
	/* Size window to picture */
	
	pictRect = (*helpPict)->picFrame;
	
	SizeWindow( helpWind, pictRect.right, pictRect.bottom, 0 );
	
	/* Open window, set grafport, and draw about picture */
	
	ShowWindow( helpWind );
	SelectWindow( helpWind );
	
	SetPort( helpWind );
	
	DrawPicture( helpPict, &pictRect );
	
	/* Setup our private BAT record */
	
	helpFigs->active = 1;
	helpFigs->period = 2;
	helpFigs->numFrames = 4;
	for(frame=0;frame<4;frame++) 
		helpFigs->frames[frame] = &boxs[frame];
	helpFigs->position.h = 34;
	helpFigs->position.v = 80;

	helpFigs[1].active = 1;
	helpFigs[1].period = 20;
	helpFigs[1].numFrames = 2;
	for(frame=0;frame<2;frame++) 
		helpFigs[1].frames[frame] = &mines[frame];
	helpFigs[1].position.h = 148;
	helpFigs[1].position.v = 80;

	helpFigs[2].active = 1;
	helpFigs[2].period = 2;
	helpFigs[2].numFrames = 7;
	for(frame=0;frame<7;frame++) 
		helpFigs[2].frames[frame] = &meanies[frame];
	helpFigs[2].position.h = 34;
	helpFigs[2].position.v = 117;

	helpFigs[3].active = 1;
	helpFigs[3].period = 7;
	helpFigs[3].numFrames = 5;
	for(frame=0;frame<5;frame++) 
		helpFigs[3].frames[frame] = &meanMines[frame];
	helpFigs[3].position.h = 148;
	helpFigs[3].position.v = 117;

	BATinit( helpFigs, 4, helpWind, &helpBat );
	
	/* Wait for a mouse down event inside the window */
	
	FlushEvents( everyEvent, 0 );
	lastTick = TickCount();
		
	for(;;) {
/*		while( !GetNextEvent( mDownMask|updateMask, &doneEvent )) { */
		while( !GetNextEvent( mDownMask, &doneEvent )) {
			thisTick = TickCount();
			if (thisTick&&0x2F == 0x2F) SystemTask();
			if (thisTick != lastTick) {
				lastTick = thisTick;
				BATnext( &helpBat );
			}
		}
		
		if (doneEvent.what==mouseDown) {
			whereCode = FindWindow( doneEvent.where, &whichWindow );
			if (whereCode == inContent && whichWindow == helpWind )
				break;
			else
				SysBeep( 30 );
		}
		else {
			BeginUpdate( helpWind );
			DrawPicture( helpPict, &pictRect );
			BATnext( &helpBat );
			EndUpdate( helpWind );
		}

	}
	
	/* Kill the window and the picture and return grafport */

	ReleaseResource( helpPict );	
	DisposeWindow( helpWind );
	
	SetPort( savePort );
	
	return;
}

