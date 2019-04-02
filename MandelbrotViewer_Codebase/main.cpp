#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <winbgim.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>

#define APP_WIDTH 1430 //1430
#define APP_HEIGHT 750 //750
#define ITERATIONS 50 //The base number of iterations for the mandelbrot set.
#define MAX_ITERATIONS 2000

using namespace std;

class mandel{
public:
    mandel(){centerX=0;centerY=0;xRange=4;zoomFactor=300;colorScheme=1;}
    void drawSet();//Draw the entire fractal onto the screen
    void drawPoint(int, int, int);//Draw a specific point onto the screen depending on how many iterations it took it to escape the set
    int iterations(double, double);//The number of iterations a point took to escape the set
    void setOrgin(double,double);//Set the center of the screen to a (real, imaginary) point
    void setZoom(double);//Zoom into the set by this much. Greater than 1 means zoom in, less than 1 is zoom out.
    void redraw(int,bool);//Either reset the zoom and orgin of the set and redraw OR change the colorscheme and redraw
private:
    double centerX;//Where the camera is looking in X
    double centerY;//Where the camera is looking in Y
    double xRange;//The difference in the real values from the leftmost point to the rightmost
    double zoomFactor;//The larger the number, the more zoomed in we are into the set
    int iterationActual;//The deeper into the set we go, the more iterations needed. This is the number of iterations needed.
    int colorScheme;
};

int mandel::iterations(double x,double y){//This is where the actual mandelbrot set algoritm is
    double real=0,imag=0;
    double tmp, realSqr=0, imagSqr=0;
    for(int i=0;i<iterationActual;i++){//This doesn't look like the algoritm, as it is heavily modified to be more efficient
        imag=real*imag;
        imag+=imag;
        imag+=y;
        real=realSqr-imagSqr+x;
        realSqr=pow(real,2);
        imagSqr=pow(imag,2);
        if(realSqr+imagSqr>4){//If the point is going to converge to infinity, stop it right away
            return iterationActual-i;
            i=iterationActual;//Stop the loop
        }
    }
    return 0;//The point is in the set
}

void mandel::drawPoint(int row, int column, int iterationNumber){
    if(iterationNumber>0){//If the iteration number is 0, it is a part of the mandlebrot set. Therefore, leave it black
        int theColor;
        switch (colorScheme){//The color of the set depends on the colorScheme of the set
        case 1://Blue background, white glow
            theColor=305-((double)iterationNumber)/iterationActual*255;
            if(theColor>255){
                theColor=255;
            }
            setcolor(COLOR(theColor,theColor,200+theColor*0.1));
            break;
        case 2://Black background, white glow
            theColor=305-((double)iterationNumber)/iterationActual*255;
            if(theColor>255){
                theColor=255;
            }
            setcolor(COLOR(theColor,theColor,theColor));
            break;
        case 3://Turquoise background, red glow
            theColor=305-((double)iterationNumber)/iterationActual*255;
            if(theColor>255){
                theColor=255;
            }
            setcolor(COLOR(theColor,128-theColor/2,128-theColor/2));
            break;
        case 4://Yellow background, blue glow
            theColor=((double)iterationNumber)/iterationActual*255;
            theColor=theColor*theColor/255;
            if(theColor>255){
                theColor=255;
            }
            setcolor(COLOR(theColor,64+theColor/1.6,128));
            break;
        case 5://Pink background, yellow glow
            theColor=305-((double)iterationNumber)/iterationActual*255;
            if(theColor>255){
                theColor=255;
            }
            setcolor(COLOR(180,theColor,128-theColor/2));
            break;
        }
        rectangle(column,row,column+2,row+2);//Draw the point onto the screen
    }
}

void mandel::drawSet(){
    iterationActual=ITERATIONS*(1+sqrt(sqrt(zoomFactor))/5);//The iterationsActual is proportional to the base Iterations and the zoomFactor
    if(iterationActual>MAX_ITERATIONS){//If we do too many iterations, the program runs very slow
        iterationActual=MAX_ITERATIONS;
    }
    xRange=APP_WIDTH/zoomFactor;//The range on the real axis
    double yRange=xRange*APP_HEIGHT/APP_WIDTH;//The range on the imaginary axis. It is the real axis range * the ratio of the apps height to the width
    double theX,theY;//The points x and y corresponding real and imaginary value
    double leftBound=centerX-(xRange/2);//The leftmost pixel's real value
    double bottomBound=centerY-(yRange/2);//The bottommost pixel's imaginary value
    for(int xDistplace=0;xDistplace<8;xDistplace+=4){
        for(int yDistplace=0;yDistplace<8;yDistplace+=4){
            for(int row=xDistplace;row<APP_HEIGHT;row+=8){//Draw every 8th pixel. This drastically increases preformance
                theY=bottomBound+(row)/zoomFactor;//The corresponding imaginary componant to the point's y value
                for(double column=yDistplace;column<APP_WIDTH;column+=8){
                    theX=leftBound+column/zoomFactor;//The corresponding real componant to the point's x value
                    //Put the point into the iterations algoritm and give the result as the color aregument for the drawPoint function
                    drawPoint(row,column,iterations(theX,theY));
                }
            }
        }
    }
}

void mandel::setOrgin(double mouseX,double mouseY){
    double yRange=xRange*APP_HEIGHT/APP_WIDTH;
    mouseX*=xRange/APP_WIDTH;//Convert the x position of the mouse from pixels to real numbers
    mouseY*=yRange/APP_HEIGHT;//Convert the y position of the mouse from pixels to imaginary numbers

    double leftBound=centerX-(xRange/2);//The leftmost pixel's real value
    double bottomBound=centerY-(yRange/2);//The bottommost pixel's imaginary value
    centerX=leftBound+mouseX;
    centerY=bottomBound+mouseY;
}

void mandel::setZoom(double zoomRatio){
        zoomFactor*=zoomRatio;//Zoom into the set by a factor of the zoomRatio
}

void mandel::redraw(int theColor, bool reset){//-1 for the color means keep the current
        if(reset){
            zoomFactor=300;
            centerX=0;
            centerY=0;
        }
        if(theColor!=-1){
            colorScheme=theColor;
        }
}

void doubleBuffer(){//This removes the ugly flashing effect. It does this by switching pages
    if(getvisualpage()==1){
        setvisualpage(2);
        setactivepage(1);
    }else{
        setvisualpage(1);
        setactivepage(2);
    }
}

void initTextPage(bool optionsUp){//Draw the options text if optionsUp. Otherwise, only draw 'o' for options
    setcolor(15);
    settextstyle(8,HORIZ_DIR,1);//Set the font

    if(optionsUp){
        outtextxy(APP_WIDTH-300,30,"Left/Right Click to zoom");
        outtextxy(APP_WIDTH-300,65,"'r' to reset");
        outtextxy(APP_WIDTH-300,100,"'1-5' for recolor");
    }else{
        outtextxy(APP_WIDTH-220,30,"'o' for options");
    }
}

int main(){
    bool done=false;
    double mouseClickX,mouseClickY;
    bool redraw=false;//Set this to true when we want to reset to the original position
    bool optionsUp=false;
    char input;
    mandel theSet;
    initwindow(APP_WIDTH,APP_HEIGHT);
    setwindowtitle("Mandelbrot Set Viewer");
    while(!done){
        setbkcolor(0);
        theSet.drawSet();
        initTextPage(optionsUp);//If the optionsUp is false, only draw the 'o' for options text
        while(!ismouseclick(WM_LBUTTONDOWN)&&!ismouseclick(WM_RBUTTONDOWN)&&!redraw){//Do nothing untill the user clicks
            if(kbhit()){//Character input commands
                input=(char)getch();
                switch (input){
                case 'r'://Redraw the fractal
                    theSet.redraw(-1,true);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
                case 'o':
                    optionsUp=true;
                    initTextPage(optionsUp);//If the optionsUp is false, only draw the 'o' for options text
                    break;
                case '1':
                    theSet.redraw(1,false);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
                case '2':
                    theSet.redraw(2,false);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
                case '3':
                    theSet.redraw(3,false);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
                case '4':
                    theSet.redraw(4,false);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
                case '5':
                    theSet.redraw(5,false);//Reset the zoom and orgin of the camera
                    redraw=true;
                    break;
            }
            delay(10);
            }
        }
        if(!redraw){//If we didn't left or right click, then don't move the orgin to mouse click and don't zoom
            mouseClickX=mousex();
            mouseClickY=mousey();
            if(ismouseclick(WM_LBUTTONDOWN)){//The user left clicked, zoom into the fractal
                theSet.setOrgin(mouseClickX,mouseClickY);
                theSet.setZoom(2.0);
            }else{
                theSet.setOrgin(APP_WIDTH-mouseClickX,APP_HEIGHT-mouseClickY);
                theSet.setZoom(0.5);
            }
            clearmouseclick(WM_LBUTTONDOWN);
            clearmouseclick(WM_RBUTTONDOWN);
        }
        redraw=false;//Don't redraw the set over and over again, once is enough
        optionsUp=false;//Close the options menu
        cleardevice();

    }
    return 0;
}
