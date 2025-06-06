//usr/bin/gcc $0 -o $0.bin -lSDL2 && ./$0.bin;exit
#include <time.h>
#include <SDL2/SDL.h>
#define I(x,y)int x=y;
void P(SDL_Renderer* R,int x,int y,int r,int g,int b){SDL_SetRenderDrawColor(R,r
*3/4,g*3/4,b*3/4,255);SDL_Rect A={x,y,16,16};SDL_RenderFillRect(R,&A);SDL_SetRe\
nderDrawColor(R,r,g,b,255);SDL_Rect B={x+2,y+2,12,12};SDL_RenderFillRect(R,&B);}
int main(){SDL_Init(SDL_INIT_VIDEO);I(w,0)I(v,0)I(x,1)I(y,0)I(l,1)I(f,16+16*(ra\
nd()%(38)))I(h,16+16*(rand()%28))I(r,1)int X[1200];int Y[1200];X[0]=Y[0]=0;sran\
d(time(0));int i,j;SDL_Window*W=SDL_CreateWindow("snek",SDL_WINDOWPOS_CENTERED,
SDL_WINDOWPOS_CENTERED,640,480,0);SDL_Renderer*R=SDL_CreateRenderer(W,-1,SDL_RE\
NDERER_ACCELERATED);const char* K;while(r){SDL_Event E;while(SDL_PollEvent(&E)){
if(E.type==SDL_QUIT)r=0;}K = SDL_GetKeyboardState(0);if(K[SDL_SCANCODE_LEFT]&!x)
x=-1,y=0;if(K[SDL_SCANCODE_RIGHT]&!x)x=1,y=0;if(K[SDL_SCANCODE_UP]&!y)x=0,y=-1;
if(K[SDL_SCANCODE_DOWN]&!y)x=0,y=1;w+=x*16;v+=y*16;if(w<0|v<0|w>=624|v>=464)r=0;
for(i=l-1;i>0;i--)if(w==X[i]&&v==Y[i])r=0;else X[i]=X[i-1],Y[i]=Y[i-1];X[0]=w,Y
[0]=v;if( f==w && h==v)l+=1,X[l-1]=w,Y[l-1]=v,f=16+16*(rand()%38),h=16+16*(rand
()%28);for(i=0;i<640;i+=16)for(j=0;j<480;j+=16)P(R,i,j,33,33,33);for(i=0;i<l;i\
++)P(R,X[i],Y[i],0,200,0);P(R,f,h,200,0,0);SDL_RenderPresent(R);SDL_Delay(65);
}printf("score: %d\n",l);}
