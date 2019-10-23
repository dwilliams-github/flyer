/*
 | Flyer project
 |
 | Definitions
 |
 | (c) Copywrite 1990   David C. Williams
 |
*/

/* Mathematical constants */

#define	PI_16	0.19634954
#define PI2 	6.2832953
#define	PI		3.1415926
#define PI23	2.094395102

/* Game constants */

#define		GAME_BURST		0.25
#define		GAME_MAXSPEED	24.0
#define		GAME_LIVES		6
#define		GAME_DRAG 		{1.0, .998, .996}
#define		GAME_NMIS		4
#define		GAME_MLIFE		120
#define		GAME_MSPEED		6.4
#define		GAME_NFOE		3
#define		GAME_FVALUE		{{100,200,400},{100,200,400},\
                        	 {200,300,600},{300,500,900}}
#define		GAME_FSPEED		{{1.2,2.0,2.4},{1.5,2.2,2.8}}
#define		GAME_FANGV		{0.031, 0.052}   /* PI2/200, PI2/150 */
#define		GAME_FSLEEP		{120,90,60}
#define		GAME_FMSPD		{{0.5,0.8,1.0},{1.0,1.5,3.0}}


/* Resource ids */

#define		MAIN_WIND		128

#define		ABOUT_WIND		129
#define		ABOUT_PICT		128

#define		ABOUT_CNTL_QUIT	128
#define		ABOUT_CNTL_HELP	129

#define 	HELP_WIND		130
#define		HELP_PICT		129

#define		APPLE_MENU		128
#define		APPLE_ABOUT		1

#define		FILE_MENU		129
#define		FILE_PAUSE		1
#define		FILE_QUIT		2

#define		EDIT_MENU		130

#define		OPTION_MENU		131
#define		OPTION_CLRTT	4

#define		LEVEL_MENU		132
#define		LEVEL_BEGINNER	1
#define		LEVEL_HACKER	2
#define		LEVEL_EXPERT	3

#define		TOPTEN_DIAL		128
#define		TT_NEWGAME		1
#define		TT_QUIT			2
#define		TT_FIRSTTOP		3
#define		TT_FIRSTSCR		16
#define		TT_OL_NEWGAME	13

#define		INITS_DIAL		129
#define		IN_OKAY			1
#define		IN_INITS		2
#define		IN_OL_OKAY		6

#define		CLEAR_ALRT		128

/* Definition of (default) key codes */

#define 	KEY_TURNRIGHT	0x2f
#define		KEY_TURNLEFT	0x2e
#define		KEY_THRUST		0x78
#define		KEY_FIRE		0x7a

#define		KEY_SPACE		0x20

/* Macros */

#define		KEEP_IN_WINDOW( point, width, height ) \
     if (point->x < 0) 	    point->x += width;  \
     if (point->x > width)  point->x -= width;  \
     if (point->y < 0)      point->y += height; \
     if (point->y > height) point->y -= height;
     
/* Structure for 'scrs' resource */

typedef	struct {
	long		score;
	char	inits[3];
} OneTopTen;

typedef OneTopTen	TopTen[10];
typedef	TopTen		*TopTenPtr;
typedef	TopTenPtr	*TopTenHndl;

/* Main game record */

typedef struct {
	int			type,
				rewake,
				dying;
	double		x, y;
	double		angle;
} FoeType;

typedef struct {
	int			awake;
	double		x, y;
	double		vx, vy;
} MisType;

typedef struct {
	WindowPtr	window;
	long		score,
				alive;
	int         nlife,
				dying,
	            paused,
	            level;
	double		winWidth,
	            winHeight;
	int			angle;
	double		x,y,
				vx, vy,
				vtot;
	FoeType		foes[GAME_NFOE];
	MisType		miss[GAME_NMIS];
} FlyerViewType;