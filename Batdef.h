/*
 | BAT: Bit Animation tool: facility for small, bit mapped animation
 |
 | Facility definitions
 |
 | (c) Copywrite 6/2/90  David C. Williams   All rights reserved
*/


typedef struct BATframeSt {
	Point		hotSpot;			/* "Hot-spot" position within bit picture */
	short		bits[16];			/* 16x16 bit picture */
} BATframe;

typedef struct BATsaveSt {
	Point		aligned;			/* Position of upper left in port coordinates */
	short		active;				/* True if this save used */
	short		bits[16];			/* 16x16 bit picture */
} BATsave;

typedef struct BATfigureSt {
	short		active;				/* Figure control: 1 indicates active, 2 indicates short life */
	int			localCount;         /* Local animation counter */
	Point		position;			/* Position of figure "hot-spot" in global coordinates */
	short		period;				/* Number of 60 Hz periods for each frame */
	short		currFrame;			/* Current frame number */
	short		numFrames;			/* Number animation stages */
	BATsave		save[2];			/* Previous image saves */
	BATframe	*frames[16];		/* Each frame, in order */
} BATfigure;

typedef	struct	BATrecordSt {
	int			numFig;
	unsigned long	count;
	WindowPtr	window;
	BATfigure	*figures;
} BATrecord;

/* Function prototypes */

int	BATinit( BATfigure *BatFigs, int numFigs, 
             WindowPtr window, BATrecord *batRecord );

int	BATnext( BATrecord *batRecord );

