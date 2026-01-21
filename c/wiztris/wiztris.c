#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>
#include <string.h>

typedef uint8_t byte;

const char scoreFilename[] = "WIZTRISHI";
#define FILETYPE_SCORE 0x0020

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
#define DRAW_BACKGROUND WRITE_WHITE
#else
#define DRAW_BACKGROUND WRITE_BLACK
#endif
#define DRAW_FOREGROUND WRITE_REVERSE

#define MAXHIGH 10

typedef struct
{
    long score;
    char initials[4];
} score_t;

char last_initials[4] = "";

score_t highscores[MAXHIGH] = { {0,""} };
byte numhigh=0;
byte scores_changed=0;
void save_scores(void);

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

static char outBuffer[70];

char* gameName="Wiztris 1.96";

void drawTextAt(uint16_t x, uint16_t y, char* s) {
	setTextXY(x,y);
	putText(s);
}

void showlevel()
{
    drawTextAt(0,0,gameName);
	setTextXY(0,2);
	sprintf(outBuffer, "Level: %d", level);
	putText(outBuffer);
}

void addtoscore(int s)
{
    thescore += s;
	setTextXY(0,3);
	sprintf(outBuffer, "Score: %ld", thescore);
	putText(outBuffer);
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
    showlevel();
    if(numhigh)
    {
        sprintf(outBuffer,"%s %ld",highscores[0].initials,highscores[0].score);
		setTextXY(0,5);
		putText(outBuffer);
    }
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

void getinitials(char* out, uint16_t length, uint16_t col, uint16_t row) {
	uint16_t w = getFontWidth();
	uint16_t h = getFontLineHeight();
	uint16_t x = col * w;
	uint16_t y = row * h;
	
	*SCREEN_MEMORY_CONTROL = DRAW_FOREGROUND;
	drawHorizontalLine(x-2,y-2,x+w*length);
	drawHorizontalLine(x-2,y+h+1,x+w*length);
	drawVerticalLine(x-2,y-2,y+h+1);
	drawVerticalLine(x+w*length+1,y-2,y+h+1);
	
	memset(out, 0, length+1);
	strcpy(out, last_initials);	
	uint16_t have = strlen(out);
	uint16_t cursorPosition = 0;
	
	while(1) {
		if (have>length)
			have = length;
		if (have==0) {
			cursorPosition = 0;
		}
		else if (have == length) {
			cursorPosition = length - 1;
		}
		else {
			cursorPosition = have;
		}
		char c = out[cursorPosition];
		if (c == 0) {
			c = ' ';
			out[cursorPosition] = ' ';
		}

		setTextXY(col,row);
		for (int i=0;i<length;i++) {
			setTextReverse((cursorPosition==i));
			putChar(out[i]);
		}
		setTextReverse(0);
		uint16_t k;
		while (! (k = getKey())) ;
		switch(k) {
			case KEY_TURN_CW:
				if (c < 'A' || c > 'Z') {
					c = 'A';
				}
				else {
					c++;
					if (c > 'Z')
						c = 'A';
				}
				out[cursorPosition] = c;
				break;
			case KEY_TURN_CCW:
				if (c < 'A' || c > 'Z') {
					c = 'Z';
				}
				else {
					c--;
					if (c < 'A')
						c = 'Z';
				}
				out[cursorPosition] = c;
				break;
			case KEY_RUN:
				drawTextAt(col,row,out);
				for (int i=strlen(out);i<length;i++) putText(" ");
				return;
			case KEY_SELECT:
				have++;
				if (have>length) {
					out[length] = 0;
					drawTextAt(col,row,out);
					return;
				}
				break;
			case KEY_CLEAR:
				if(have>0) {
					have--;
					out[have] = 0;
				}
				else {
					out[cursorPosition] = ' ';
				}
				break;
			case KEY_A:
			case KEY_B:
			case KEY_C:
			case KEY_D:
			case KEY_E:
			case KEY_F:
			case KEY_DONT_CARE:
				out[cursorPosition] = parseKey(k);
				have++;
				break;
		}
	}
}

void add_high_score(void)
{
    char initials[4] = "";
	
	drawTextAt(0,9, "Initials:");	
	drawTextAt(0,11, "SELECT: Next, RUN: Done");
	getinitials(initials, 3, 10, 9);
	strcpy(last_initials,initials);

	int16_t i;
    for(i=MAXHIGH-1;i>=0 && thescore>highscores[i].score;i--) ;
    i++;
    if(numhigh<MAXHIGH) numhigh++;
    for(int16_t j=numhigh-1;j>i;j--)
    {
        highscores[j].score=highscores[j-1].score;
        strcpy(highscores[j].initials,highscores[j-1].initials);
    }
    highscores[i].score=thescore;
    strcpy(highscores[i].initials,initials);
	scores_changed = 1;
#if 0	
	int16_t y = getTextRows()-1;
	drawTextAt(0,y,"saving");
	save_scores();
	drawTextAt(0,y,"      ");
#endif	
}

void load_scores(void)
{
    int handle;
	
	handle = openFile(scoreFilename, FILETYPE_SCORE, READ_FILE);
	
	if (handle < 0)
		return;
	
	if (sizeof(numhigh) != readFile(handle, &numhigh, sizeof(numhigh)) ||
		sizeof(highscores) != readFile(handle, highscores, sizeof(highscores))) {
		numhigh = 0;
	}
	
	readFile(handle, last_initials, sizeof(last_initials)-1);

	closeFile(handle);
}	

void save_scores(void)
{
    if(!scores_changed) return;
	
	
	drawTextAt(0,getTextRows()-1,"Saving...");
	
	int handle = openFile(scoreFilename, FILETYPE_SCORE, WRITE_FILE);
	
	if (handle < 0) {
		drawTextAt(0,getTextRows()-1,"Error    ");
		waitSeconds(1);
		return;
	}
	
	writeFile(handle, &numhigh, sizeof(numhigh));
	writeFile(handle, highscores, sizeof(highscores));
	writeFile(handle, last_initials, sizeof(last_initials)-1);
	last_initials[3] = 0;
	
	closeFile(handle);
	scores_changed = 0;
	drawTextAt(0,getTextRows()-1,"         ");
	
}



int main()
{
	char randomized = 0;
	
	*LAST_KEY = 0;

	patchVBL();	
	atexit(reload);
	setTextColors(DRAW_FOREGROUND,DRAW_BACKGROUND);
	setTextReverse(0);
	setKeyRepeat(20,8);
	load_scores();
	atexit(save_scores);

    uint16_t k;
    do {
      shownext=0;
      cls();
	  drawTextAt(0,0,gameName);
	  drawTextAt(0,2,"Copyright (c) 2002-26 Alexander Pruss");
	  drawTextAt(0,5,"In-game controls:");
	  drawTextAt(2,6,"      0/.: move");
	  drawTextAt(2,7,"      1/2: rotate");
	  drawTextAt(2,8,"      CHS: drop");
	  drawTextAt(2,9,"      Run: level up");
	  drawTextAt(2,10,"  Display: show next");
	  drawTextAt(2,11,"     Stop: exit");
	  
	  drawTextAt(0,15,"0-9:  start at given level [3=default]");
	  drawTextAt(0,16,"Stop: exit");
	  if(numhigh)
      {
		uint16_t x = getTextColumns()/2+10;
        drawTextAt(x,0,"HIGH SCORES:");
        for(uint16_t i=0;i<numhigh;i++)
        {
           drawTextAt(x,2+i,highscores[i].initials);
           sprintf(outBuffer,"%-6ld",highscores[i].score);
		   drawTextAt(x+4,2+i,outBuffer);
        }
      }	  
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
      if(thescore<=highscores[MAXHIGH-1].score) {
		 drawTextAt(0,7,"Game over!");
		 drawTextAt(0,8,"Again? (0/1)");
		 while (!(k = getKey()));
	  }
      else
      {
         drawTextAt(0,7,"High score!");
         add_high_score();
		 k = 0;
      }
   } while(k!=KEY_0 && k!=KEY_STOP);
   return 0;
}
  