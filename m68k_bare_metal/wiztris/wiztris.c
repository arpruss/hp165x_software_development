#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

typedef uint8_t byte;

#define SQUARE blacksquare

void SQUARE(uint16_t x, uint16_t y);

#define NROTS 4
#define NSHAPES 7
#define shapetbl(i,j,k) _shape[(k)+4*(j)+16*(i)]
char _shape[112]; /* [4*4*7]; */
#define GWIDTH 10
#define GHEIGHT 22
#define GHEIGHT_1 21
char grid[GHEIGHT][GWIDTH]={{0}};
#define SQUARE_HEIGHT 17
#define SQUARE_WIDTH  ADJUST_WIDTH(SQUARE_HEIGHT) // 19
#define BOARD_X ((SCREEN_WIDTH-SQUARE_WIDTH*GWIDTH)/2)


//#include "ibm8x14.c"

#define BLACK_ON_WHITE 0

#if BLACK_ON_WHITE
#define DRAW_FOREGROUND WRITE_WHITE
#define DRAW_BACKGROUND WRITE_BLACK
#else
#define DRAW_FOREGROUND WRITE_WHITE
#define DRAW_BACKGROUND WRITE_BLACK
#endif

#define HEADER 43

#define REPEAT_DELAY 20
#define REPEAT_SPEED  8

#define MAXHIGH 10

typedef struct
{
    long score;
    char initials[4];
} score_t;

score_t highscores[MAXHIGH];
byte numhigh=0;

int maxheight;
int maxwidth;
int shownext;

long thescore=0;

int level;
int scores;
// TODO: don't make the delays be longer when moving!!!
#define DELAY_BASE 279 // 9.46sec on level drop in emulation of original DOS falling block game
#define DELAY_TICK(x) ((x)*26/280) 
uint32_t delayInTicks[10]={ DELAY_TICK(280), DELAY_TICK(262), 
	DELAY_TICK(233), DELAY_TICK(210), DELAY_TICK(175), 
	DELAY_TICK(163), DELAY_TICK(140), DELAY_TICK(105), 
	DELAY_TICK(81), DELAY_TICK(52) };
//int delays[10]={ DELAY_BASE*280, DELAY_BASE*262, DELAY_BASE*233, DELAY_BASE*210, DELAY_BASE*175, DELAY_BASE*163, DELAY_BASE*140,
//DELAY_BASE*105, DELAY_BASE*81, DELAY_BASE*52 };
int fulls;

char* gameName="Wiztris 1.96";

void drawTextAt(uint16_t x, uint16_t y, char* s) {
	setTextXY(x,y);
	putText(s);
}

void showlevel()
{
    drawTextAt(0,0,gameName);
	setTextXY(0,2);
	printf("Level: %d", level);
}

void addtoscore(int s)
{
    thescore += s;
	setTextXY(0,3);
    printf("Score: %ld",thescore);
}

void init()
{
    thescore=0;
    fulls=0;
    maxheight=GHEIGHT;
	maxwidth=GWIDTH;
	for(int i=0;i<GHEIGHT;i++)
		for (int j=0;j<GWIDTH;j++)
			grid[i][j] = 0;
    for(int i=0;i<112;i++) _shape[i]=0;
#include "tristbl.c"
}

// TODO: assembly
void dosquare(int i,int j,int c)  /* row,column,color */
{
	uint16_t x = BOARD_X + j*SQUARE_WIDTH;
	uint16_t y = (GHEIGHT_1-i)*SQUARE_HEIGHT;

	if (c) 
		*SCREEN_MEMORY_CONTROL = DRAW_FOREGROUND;
	else
		*SCREEN_MEMORY_CONTROL = DRAW_BACKGROUND;
	
	SQUARE(x,y);
}

void dopiece(int row0,int col0,int n,int r,int c)
{
    int row;
    int col;
    int colmask;
    int st;
    for(row=0; row<4; row++)
    {
        st=shapetbl(n,row,r);
        if(st && row0-row>=0 && row0-row<GHEIGHT)
        {
            colmask=1;
            for(col=0; col<4; col++)
            {
                if((colmask&st) && col+col0>=0 && col+col0<maxwidth)
                    dosquare(row0-row,col+col0,c);
                colmask=colmask<<1;
            }
        }
    }
}

void drawgrid(char erase)
{
    int row;
    int col;
	if (erase) {
		for(row=0; row<GHEIGHT; row++) for(col=0; col<GWIDTH; col++) {
			char g = grid[row][col];
			if (g) dosquare(row,col,0);
		}
	}
	else {
		for(row=0; row<GHEIGHT; row++) for(col=0; col<GWIDTH; col++) {
			char g = grid[row][col];
			dosquare(row,col,g);
		}
	}
}

int fitq(int row0,int col0,int piece,int rot)
{
    int row,col,colmask,st;
    for(row=0; row<4; row++)
    {
        if(row0-row>=GHEIGHT) continue;
        st=shapetbl(piece,row,rot);
        if(st)
        {
            if(row0-row<0) return 0;
            colmask=1;
            if(st) for(col=0; col<4; col++)
            {
                if((colmask&st) &&
                (col+col0<0 || col+col0>=GWIDTH || grid[row0-row][col+col0]))
                    return 0;
                colmask=colmask<<1;
            }
        }
    }
    return 1;
}

int addtogrid(int row0,int col0,int piece,int rot)
{
    int row,col,colmask,st;
    for(row=0; row<4; row++)
    {
        colmask=1;
        st=shapetbl(piece,row,rot);
	if(st) for(col=0; col<4; col++)
	{
	    if((colmask&st) && row0-row>=0 && row0-row<GHEIGHT &&
	    col+col0>=0 && col+col0<GWIDTH)
		grid[row0-row][col+col0]=1;
	    colmask=colmask<<1;
	}
    }
    return 1;
}

void killrow(int i)
{
    int i2,j;
    for(j=0;j<GWIDTH;j++) dosquare(i,j,0);
    for(i2=i;i2<GHEIGHT-1;i2++) for(j=0;j<GWIDTH;j++)
      grid[i2][j] = grid[i2+1][j];
    for(j=0;j<GWIDTH;j++) grid[GHEIGHT-1][j]=0;
    drawgrid(0);
    fulls++;
    if(fulls>=10 && level<9)
    {
      fulls-=10;
      level++;
      showlevel();
    }
}

void scanlines(int row)
{
    int i,j;
    int full;
    i=row-4;
    if(i<0) i=0;
    for(;i<=row;i++)
    {
       full=1;
       for(j=0;j<GWIDTH;j++)
         if(!grid[i][j])
         {
            full=0;
            break;
         }
       if(full)
       {
         killrow(i);
         row--;
         i--;
       }
    }
}

int getlowestrow(int row,int piece,int rot)
{
    int i;
    for(i=3;i>=0;i--)
      if(shapetbl(piece,i,rot)) return row-i;
    return row;//should not be reached
}

int gethighestrow(int row,int piece,int rot)
{
    int i;
    for(i=0;i<4;i++)
      if(shapetbl(piece,i,rot)) return row-i;
    return row;//should not be reached
}


void doshownext(int piece,int rot,int c)
{
    maxwidth=GWIDTH+8;
    dopiece(GHEIGHT-4,GWIDTH+4,piece,rot,c);
    maxwidth=GWIDTH;
}

void cls() {
  *SCREEN_MEMORY_CONTROL = DRAW_BACKGROUND;
  fillScreen();
}
 
void help()
{
    cls();
	drawTextAt(0,0,"HELP");
}

void drawbox() {
	*SCREEN_MEMORY_CONTROL = DRAW_FOREGROUND;
	for (int i=0;i<2;i++) {
		drawVerticalLine(BOARD_X-2-i,0,GHEIGHT*SQUARE_HEIGHT);
		drawVerticalLine(BOARD_X+GWIDTH*SQUARE_WIDTH+i,0,GHEIGHT*SQUARE_HEIGHT);
		drawHorizontalLine(BOARD_X-3,GHEIGHT*SQUARE_HEIGHT+i,BOARD_X+GWIDTH*SQUARE_WIDTH+1);
	}
}

void drop()
{
    unsigned c;
    int piece;
    int rot;
    int row,col;
    int rot2;
    int col2;
    uint32_t curdelay;
    int droppedfrom;
    int nextrot,nextpiece;
    int curheight;
    rot=rand()%NROTS;
	piece=rand()%NSHAPES;
    col=3;
//    while(ozkeylower(0xff));
    showlevel();
    curheight=GHEIGHT_1-getlowestrow(0,piece,rot);
	drawbox();
    while(fitq(curheight,col,piece,rot))
    {
        nextrot=rand()%NROTS;
        nextpiece=rand()%NSHAPES;
        if(shownext) doshownext(nextpiece,nextrot,1);
        curdelay=delayInTicks[level];
        droppedfrom=-1;
        for(row=curheight; row>=0; row--)
        {
		   setVBLCounter(0);
           dopiece(row,col,piece,rot,1);
           while(getVBLCounter() < curdelay) {
			  c = getKey();
              if(c) {
				 //*LAST_KEY = 0;
				 switch(c)
                 {
                   case KEY_DISPLAY:
                            shownext=!shownext;
                            doshownext(nextpiece,nextrot,shownext);
                            break;
                   case KEY_2:
				   case KEY_TURN_CW:
                            rot2=(rot+NROTS-1)%NROTS;
                            if(fitq(row,col,piece,rot2))
                            {
                               dopiece(row,col,piece,rot,0);
                               rot=rot2;
                               dopiece(row,col,piece,rot,1);
                            }
                            break;
                   case KEY_1:
				   case KEY_TURN_CCW:
                            rot2=(rot+1)%NROTS;
                            if(fitq(row,col,piece,rot2))
                            {
                               dopiece(row,col,piece,rot,0);
                               rot=rot2;
                               dopiece(row,col,piece,rot,1);
                            }
                            break;
                   case KEY_RUN:
                            if(level<9)
                              {
                                level++;
                                curdelay=delayInTicks[level];
                                showlevel();
                              }
                              break;
                   case KEY_DECIMAL:
                          col2=col+1;
                          if(fitq(row,col2,piece,rot))
                          {
                            dopiece(row,col,piece,rot,0);
                            col=col2;
                            dopiece(row,col,piece,rot,1);
                          }
                          break;
                   case KEY_0:
                      col2=col-1;
                      if(fitq(row,col2,piece,rot))
                      {
                        dopiece(row,col,piece,rot,0);
                        col=col2;
                        dopiece(row,col,piece,rot,1);
                      }
                      break;
                   case KEY_CHS:
                          curdelay=0;
                          droppedfrom=getlowestrow(row,piece,rot);
                      break;
                   case KEY_STOP:
					  _exit(0);
                      return;
                   case KEY_SELECT: {
					  uint32_t tick = getVBLCounter();
					  drawgrid(1);
					  dopiece(row,col,piece,rot,0);
					  doshownext(nextpiece,nextrot,0);
				      drawTextAt(34,10,"PAUSED");
                      getKey();
				      drawTextAt(34,10,"      ");
					  drawgrid(0);
					  dopiece(row,col,piece,rot,1);
					  doshownext(nextpiece,nextrot,shownext);
					  setVBLCounter(tick);
                      break;
				   }
                 }
			  }
			  
		   }
		   
           if(fitq(row-1,col,piece,rot))
             dopiece(row,col,piece,rot,0);
           else
           {
             if(gethighestrow(row,piece,rot)>=GHEIGHT)
             {
                return;
             }
             addtogrid(row,col,piece,rot);
             if(droppedfrom<0) droppedfrom=getlowestrow(row,piece,rot);
             if(!shownext)
               addtoscore(5+(2*level)+droppedfrom);
             else
               addtoscore(3+(3*level)/2+droppedfrom);
             scanlines(row);
             break;
           }
        }
        if(shownext) doshownext(nextpiece,nextrot,0);
        rot=nextrot;
        piece=nextpiece;
        col=3;
        curheight=GHEIGHT_1-getlowestrow(0,piece,rot);
    }
    dopiece(curheight,col,piece,rot,1);
}

void exit1(void) {
	drawTextAt(0,0,"EXIT 1");
}

void exit2(void) {
	drawTextAt(0,0,"EXIT 2");
}

int main()
{
	char randomized = 0;
	
	*LAST_KEY = 0;

	patchVBL();	
	atexit(reload);
	setTextBlackOnWhite(BLACK_ON_WHITE);
	setKeyRepeat(20,8);
	
    uint16_t k;
    do {
      shownext=0;
      cls();
	  drawTextAt(0,13-13,gameName);
	  drawTextAt(0,15-13,"Copyright (c) 2002-26 Alexander Pruss");
	  drawTextAt(0,18-13,"In-game controls:");
	  drawTextAt(2,19-13,"      0/.: move");
	  drawTextAt(2,20-13,"      1/2: rotate");
	  drawTextAt(2,21-13,"      CHS: drop");
	  drawTextAt(2,22-13,"      Run: level up");
	  drawTextAt(2,23-13,"  Display: show next");
	  drawTextAt(2,24-13,"     Stop: exit");
	  
	  drawTextAt(0,28-13,"0-9:  start at given level [3=default]");
	  drawTextAt(0,29-13,"Stop: exit");
	  
	  while (! (k = getKey()) );
	  
	  if (!randomized) {
		  srand(getVBLCounter());
		  randomized = 1;
	  }
      switch(k) {
		  case KEY_0:
			level = 0;
			break;
		  case KEY_1:
			level = 1;
			break;
		  case KEY_2:
			level = 2;
			break;
		  case KEY_3:
			level = 3;
			break;
		  case KEY_4:
			level = 4;
			break;
		  case KEY_5:
			level = 5;
			break;
		  case KEY_6:
			level = 6;
			break;
		  case KEY_7:
			level = 7;
			break;
		  case KEY_8:
			level = 8;
			break;
		  case KEY_9:
			level = 9;
			break;
	      case KEY_STOP:
			return 0;
		  default:
			level = 3;
			break;
	  }	  
      init();
      cls();
      addtoscore(0);
      drop();
	  drawTextAt(0,3,"Game over!");
      drawTextAt(0,4,"Again? (0/1)");
	  while (!(k = getKey()));
   } while(k!=KEY_0 && k!=KEY_STOP);
   return 0;
}
 