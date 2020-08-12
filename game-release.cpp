//============================================================================
// Name        : The Snake
// Author      : Noman Aziz
// Version     : 18
// Copyright   : (c) Reserved
// Description : 2D Snake Game...
//============================================================================
#ifndef TETRIS_CPP_
#define TETRIS_CPP_
#include "util.h"
#include <iostream>
#include<vector>
#include<algorithm>
//#include<cstdlib>
#include<ctime>
#include<cstdlib>
#include<string>
//#include<sys/wait.h>
//#include<stdlib.h>
//#include<stdio.h>
#include<unistd.h>
#include<sstream>
#include<cmath>
using namespace std;

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * */
void SetCanvasSize(int width, int height) 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();
}




/*
 * Main Canvas drawing function.
 * */

//DECLARATIONS

bool screen = true ;                 //If snake collides everything stops.

int coord[1000][2]={ {400,300} , {390,300} , {380,300} , {370,300} } ;      //For snakes body
int headx = coord[0][0] , heady = coord[0][1] ;                             //For snakes head
int tempx = 0 , tempy = 0 ;
int temp2x = 400, temp2y = 300 ;
int movement = 0 ;                                                          //For snake's movement
int previous = 5 ;                                                          //To check previous movement of snake


//Food coordinates 
int foodx[5],foody[5] ;              //For five foods
int food_macrox , food_macroy ;      //For Bonus food

//Some vairalbes relatred food.
int food_check=1;                   //Main condition to change food coordinates
int check[5] = { 1,1,1,1,1 } ;      //Secondary check for different food coordinates
double timer[5] = { 0,0,0,0,0 } ;   //Timer for five foods
double macro_timer = 0 ;            //Timer for Bonus food
double collision_timer = 0 ;        //Special Timer for Collision Detection of Snake with its own body. For more details check VOID TIMER.
int macro_check = 0 ;               //Secondary check for bonus food

//Some Varialbes related Obstacles
int obs_x = 300 ;                   //X-coordinate of Obstacle
int obs_y = 300 ;                   //Y-coordinate of Obstacle
double obs_timer = 0 ;              //Timer for changing Obstacle Position

//Others
int body = 3 ;              //To check size of snake i.e initially snake has 3 body parts except head
int score = 0 ;             //This counts Score of Snake after eating food
int col = 1 ;               //this is for color of Snake's Tail
int col2 = 0 ;              //This is for Color of BOnus Food
int progress = 201 ;        // progress bar
string scored ;             //To convert score and display it in DrawString FTN



//To update coordinates of Head when kefood is pressed
void move_head ( int mv, int &hx, int &hy )
{   
    if ( mv == 1 )          //Left
    {
        hx -= 10 ;
    }
    else if ( mv == 2 )     //Right
    {
        hx += 10 ;
    }
    else if ( mv == 3 )     //Up
    {
        hy += 10 ;
    }
    else if ( mv == 4 )     //Down
    {
        hy -= 10 ;
    }
}


//To change snakes position when it gets out of screen height and width
void screen_swap ( int &hx, int &hy )
{
    if ( hx > 800 )         //Right Limit
    {
        hx = 0 ;
    }
    else if ( hx < 0 )      //Left Limit
    {
        hx = 800 ;
    }
    else if ( hy < 0 )      //Down Limit
    {
        hy = 530 ;
    }
    else if ( hy > 530 )    //Up Limit
    {
        hy = 0 ;
    }
}


//To check that if snake's head has eaten food or not
void food_collision ( int hx, int hy, int fx[5], int fy[5], int &food_check_2, int check_2[5], int &size, int &score2, int &color2  )
{
    for ( int i=0 ; i<5 ; i++ )
    {
        if ( ( hx >= fx[i]-6 && hx <= fx[i]+6 )  and ( hy >= fy[i]-6 && hy <= fy[i]+6 ) )
        {
            {
                food_check_2 = 1 ;      //This check is used for Updating food's new location
                check_2[i] = 1 ;        //This is additional check for specific food
                
                //If food is eaten add another square to snake
                size++ ;
                
                //score is added
                score2 += 5 ;

                //update progress bar
                progress += 1 ;

                //color of snake is changed
                color2++;
            }
        }
    }
}


//This Check if Snake has eaten bonus food.
void macro_food_collision ( int hx, int hy, int fx, int fy, int &foodcheck, int &size, int &score2, double &macrotimer )
{
    if ( ( hx >= fx-6 && hx <= fx+6 ) && ( hy >= fy-6 && hy <= fy+6 ) )
    {
        {
            foodcheck = 1 ;
            
            //If food is eaten add two squares to snake
            size += 2 ;
            
            score2 += 20 ;
            macrotimer = 0 ;

            progress += 10 ; //Update progress bar

        }
    }
}


//To detect collision with itself
void itself_collision ( )
{
    for ( int i=3 ; i<body ; i++ ) //Here we started checking collision from fourth tail because snake's head can't colllide with first three tails.
    {
        if ( headx == coord[i][0] && heady == coord[i][1] )
        {
                screen = false ;
                DrawString ( 400, 300, "GAME OVER ! PRESS ESC TO EXIT", colors[RED] ) ;
                DrawString ( 400, 350, "YOUR SCORE = ", colors[RED] ) ;
                DrawString ( 600, 350, scored, colors[RED] ) ;
        }
    }
}


//To detect collision with hurdle
void hurdle_collision ( )
{

    for ( int i = obs_x , j = obs_y ; i <= obs_x+50 && j <= obs_y+50 ; i++ , j++ ) //Note that +50 is added because obstacle is 50units wide.
    {

        if ( headx == i && heady == j )
        {
                screen = false ;
                DrawString ( 400, 300, "GAME OVER ! PRESS ESC TO EXIT", colors[RED] ) ;
                DrawString ( 400, 350, "YOUR SCORE = ", colors[RED] ) ;
                DrawString ( 600, 350, scored, colors[RED] ) ; 
        }

    }

}


//To check and change our ( changing food locations ) condition after 15 or 20 seconds.
void random_food_condition ( int &foodcheck )
{
    if ( ( timer[0]-15  >= 0.0000001 ) || ( timer[1]-15  >= 0.0000001 ) || ( timer[2]-15  >= 0.0000001 ) || ( timer[3]-15  >= 0.0000001 ) ||
       ( ( timer[4]-15  >= 0.0000001 ) || ( timer[5]-15  >= 0.0000001 ) || ( macro_timer-60  >= 0.0000001 ) ) )
    {
        foodcheck = 1 ;
    }
}


//To check and change our specifically Bonus food appearing conditions after 60s.
void random_macro_food_condition ( int &macrocheck )
{
    if ( macro_timer-60 >= 0.0000001 && macro_timer-60.1 <= 0.0000001 )
    {
        macro_check = 1 ;
    }
}


//It generates food at random locations.
void random_food_position_generator ( int &foodcheck, int check2[5], double timer2[5], int fx[5], int fy[5], int &macrocheck, int &fmx, int &fmy )
{

    int chk1 = 0 ; //To ensure that new food wouldn't be placed on location where other foods are present
    int chk2 = 0 ; //To ensure that new food wouldn't be placed on location where snake's tail is present
    int chk3 = 0 ; //To ensure that new food wouldn't be placed on location where snake's head is present
    int chk4 = 0 ; //To ensure that new food wouldn't be placed on location where Obstacle is present

    if ( foodcheck==1 )
    {
        if ( check2[0] == 1 || timer2[0]-15  >= 0.0000001 ) //For first food
        {
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;

                fx[0] = GetRandInRange( 50, 150 ) ;
                fy[0] = GetRandInRange( 100, 400 ) ;

                if ( fy[0] != fy[1] && fy[0] != fy[2] && fy[0] != fy[3] && fy[0] != fy[4] && fy[0] != fmy ) //This checks that y-cordinates of food should not be same
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fx[0] != coord[i][0] && fy[0] != coord[i][1] ) //This checks that coordinates of food should not be equal to where snake's body is present
                        chk2++ ;
                }

                if ( fx[0] != headx && fy[0] != heady ) //This checks that coordinates of food should not be equal to where snake's head is present
                    chk3++ ;

                if ( ( fx[0] < obs_x && fx[0] > obs_x+50 ) and ( fy[0] < obs_y && fy[0] > obs_y+50 ) ) //This checks that coordinates of food should not be equal to where obstacle is present.
                    chk4++ ;

            }
                  

            foodcheck=0;        //Close the main checking condition
            check2[0] = 0 ;     //Close the secondary checking condition
            timer2[0] = 0 ;     //Reset the timer
        }

        if ( check2[1] == 1 || timer2[1]-15  >= 0.0000001 ) //For second food
        {
            
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;

                fx[1] = GetRandInRange( 200, 300 ) ;
                fy[1] = GetRandInRange( 100, 400 ) ;

                if ( fy[1] != fy[0] && fy[1] != fy[2] && fy[1] != fy[3] && fy[1] != fy[4] && fy[1] != fmy )
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fx[1] != coord[i][0] && fy[1] != coord[i][1] )
                        chk2++ ;
                }

                if ( fx[1] != headx && fy[1] != heady )
                    chk3++ ;

                if ( ( fx[1] < obs_x && fx[1] > obs_x+50 ) and ( fy[1] < obs_y && fy[1] > obs_y+50 ) )
                    chk4++ ;

            } 

            foodcheck=0;
            check2[1] = 0 ;
            timer2[1] = 0 ;
        }

        if ( check2[2] == 1 || timer2[2]-15  >= 0.0000001 ) //For third food
        {
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;
            
                fx[2] = GetRandInRange( 350, 450 ) ;
                fy[2] = GetRandInRange( 100, 400 ) ;

                if ( fy[2] != fy[0] && fy[2] != fy[1] && fy[2] != fy[3] && fy[2] != fy[4] && fy[2] != fmy )
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fx[2] != coord[i][0] && fy[2] != coord[i][1] )
                        chk2++ ;
                }

                if ( fx[2] != headx && fy[2] != heady )
                    chk3++ ;

                if ( ( fx[2] < obs_x && fx[2] > obs_x+50 ) and ( fy[2] < obs_y && fy[2] > obs_y+50 ) )
                    chk4++ ;

            }

            foodcheck=0;
            check2[2] = 0 ;
            timer2[2] = 0 ;
        }

        if ( check2[3] == 1 || timer2[3]-15  >= 0.0000001 ) //For fourth food
        {
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;

                fx[3] = GetRandInRange( 500, 600 ) ;
                fy[3] = GetRandInRange( 100, 400 ) ;

                if ( fy[3] != fy[0] && fy[3] != fy[1] && fy[3] != fy[2] && fy[3] != fy[4] && fy[3] != fmy )
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fx[3] != coord[i][0] && fy[3] != coord[i][1] )
                        chk2++ ;
                }

                if ( fx[3] != headx && fy[3] != heady )
                    chk3++ ;

                if ( ( fx[3] < obs_x && fx[3] > obs_x+50 ) and ( fy[3] < obs_y && fy[3] > obs_y+50 ) )
                    chk4++ ;

            }

            foodcheck=0;
            check2[3] = 0 ;
            timer2[3] = 0 ;
        }

        if ( check2[4] == 1 || timer2[4]-15  >= 0.0000001 ) //For fifth food
        {
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;

                fx[4] = GetRandInRange( 650, 750 ) ;
                fy[4] = GetRandInRange( 100, 400 ) ; 

                if ( fy[4] != fy[0] && fy[4] != fy[1] && fy[4] != fy[2] && fy[4] != fy[3] && fy[4] != fmy )
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fx[4] != coord[i][0] && fy[4] != coord[i][1] )
                        chk2++ ;
                }

                if ( fx[4] != headx && fy[4] != heady )
                    chk3++ ;

                if ( ( fx[4] < obs_x && fx[4] > obs_x+50 ) and ( fy[4] < obs_y && fy[4] > obs_y+50 ) )
                    chk4++ ;

            }

            foodcheck=0;
            check2[4] = 0 ;
            timer2[4] = 0 ;
        }

        if ( macrocheck == 1 ) //For bonus food
        {

            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;
            chk4 = 0 ;

            while ( chk1 != 1 && chk2 != body && chk3 != 2 && chk4 != 1 )
            {

                chk1 = 0 ;
                chk2 = 0 ;
                chk3 = 0 ;
                chk4 = 0 ;
                
                fmx = GetRandInRange( 100, 750 ) ;
                fmy = GetRandInRange( 100, 400 ) ; 

                if ( ( fmx != fx[0] && fmx != fx[1] && fmx != fx[2] && fmx != fx[3] && fmx != fx[4] ) and ( fmy != fy[0] && fmy != fy[1] && fmy != fy[2] && fmy != fy[3] && fmy != fy[4] ) )
                    chk1 = 1 ;

                for ( int i=0 ; i<body ; i++ )
                {
                    if ( fmx != coord[i][0] && fmy != coord[i][1] )
                        chk2++ ;
                }

                if ( fmx != headx && fmy != heady )
                    chk3++ ;

                if ( ( fmx < obs_x && fmx > obs_x+50 ) and ( fmy < obs_y && fmy > obs_y+50 ) )
                    chk4++ ;

            }

            foodcheck = 0 ;
            macrocheck = 0 ;
             
        }

    }
}


//For generating random positions of Obstacles.
void random_obstacles_position ( int &obsx, int &obsy, double &obstimer )
{

    int chk1 = 0 ; //To check whether obstacle is not colliding with any food.
    int chk2 = 0 ; //To check that whether obstacle is placed on snake's head or not.
    int chk3 = 0 ; //To check that whether obstacle is placed on snake's body or not.

    if ( obstimer-30 >= 0.0000001 ) 
    {

        while ( chk1 != 1 && chk2 != body && chk3 != 2 )
        {
            
            chk1 = 0 ;
            chk2 = 0 ;
            chk3 = 0 ;

            obsx = GetRandInRange ( 100, 700 ) ;
            obsy = GetRandInRange ( 100, 400 ) ;

            if (    ( obsx != foodx[0] && obsx != foodx[1] && obsx != foodx[2] && obsx != foodx[3] && obsx != foodx[4] )   and    ( obsy != foody[0] && obsy != foody[1] && obsy != foody[2] && obsy != foody[3] && obsy != foody[4] )    ) //This checks that obstacle should not be placed on food location
            {
                chk1 = 1 ;
            }

            if ( obsx != headx && obsy != heady ) //This checks that obstacle should not be placed on snake's head
            {
                chk2 = 2 ;
            }

            for ( int i=0 ; i<body ; i++ )
            {
                if ( obsx != coord[i][0] && obsy != coord[i][1] ) //This checks that obstacle should not be placed on snake's body.
                {
                    chk3++ ;
                }
            }

        }

        obstimer = 0 ; //To reset timer after 30 s
    }
}


//To draw Food.
void draw_food ( )
{
    for ( int i=0 ; i<5 ; i++ )
    {
        DrawCircle( foodx[i], foody[i], 5, colors[GREEN] );
    }
}


//To Draw Bonus Food
void draw_macro_food ( double &macrotimer )
{
    if ( macrotimer-60 >= 0.0000001 && macrotimer-75 <= 0.0000001 ) //To display this food from 1mins to 15s
    {
            DrawCircle( food_macrox, food_macroy, 5, colors[col2] );
            
            if ( macrotimer-75 >= 0.0000001 ) //After 1min and 15s the timer will reset to 0s.
            {
                macrotimer = 0 ;    //Reset the timer to 0s.
            }
    }
}


//To Continuously change colors of Bonus food when it appears.
void macro_food_rainbow_effect ( int &color2 )
{
    if ( color2 == 141 )
    {
        color2 = 0 ;
    }
    else
    {
        color2++ ; 
    }  
}


//To reset colors of tail after colors reach out of limit
void tail_color_resettor ( int &color2 )
{
    if ( color2 == 141 )
    {
        color2 = 0 ;
    }
}


//For Setting Tails to follow head and to Draw Tails
void tails_positioning_and_drawing ( int &tx, int &ty, int pos[1000][2], int &t2x, int&t2y )
{
    for ( int i=0 ; i<body ; i++ )
    {
        tx = pos[i+1][0] ;
        ty = pos[i+1][1] ;
        
        for ( int j=0 ; j<2 ; j++ )
        {
            if ( j==0 )
            {
                pos[i+1][j] = t2x ;
                t2x = tx ;
            }
            else
            {
                pos[i+1][j] = t2y ;
                t2y = ty ;
            }
        }

        DrawSquare( pos[i+1][0] , pos[i+1][1] , 10 , colors[col]); 
    }
}


void Display()      //All Details about below functions are written above along with the function. 
{
    if ( screen == true )
    {    

        glClearColor(0.7, 0.7, 0.7, 0); //--> White Colour
    
        glClear(GL_COLOR_BUFFER_BIT) ;   //Update the colors

        temp2x = headx ;
        temp2y = heady ;

        move_head ( movement, headx, heady ) ;  
    
        screen_swap ( headx, heady ) ;

        //For Drawing Head
        DrawCircle( headx+5 , heady+5 , 5, colors[BLACK]);

        tails_positioning_and_drawing ( tempx, tempy, coord, temp2x, temp2y ) ;

        itself_collision ( ) ;

        random_food_condition ( food_check ) ;

        random_macro_food_condition ( macro_check ) ;
    
        random_food_position_generator ( food_check, check, timer, foodx, foody, macro_check, food_macrox, food_macroy ) ;

        random_obstacles_position ( obs_x, obs_y, obs_timer ) ;

        //For Drawing Obstacle
        DrawLine( obs_x , obs_y ,  obs_x+50 , obs_y+50 , 10 , colors[BLACK] );

        hurdle_collision ( ) ;

        draw_food ( ) ;

        draw_macro_food ( macro_timer ) ;

        food_collision ( headx, heady, foodx, foody, food_check, check, body, score, col ) ;
    
        macro_food_collision ( headx, heady, food_macrox, food_macroy, food_check, body, score, macro_timer ) ;

        macro_food_rainbow_effect ( col2 ) ;

        tail_color_resettor ( col ) ;

        //Top Limit Line
        DrawLine( 0 , 550 ,  800 , 550 , 20 , colors[RED] );

        DrawString( 100, 560, "Progress : "  , colors[RED]);     //Displays "Progess :" at top
    
        DrawLine( 200 , 566 ,  progress , 566 , 5 , colors[BLUE] );    //Displays Progess Bar at top

        //To convert integer score data into string score data.
        scored = to_string ( score ) ;
    
        DrawString( 600, 560, "Score  =  "  , colors[BLUE]);     //Displays " Score = " at top
                                                    
        DrawString( 700, 560, scored , colors[RED]);            //Displays Actual Score at top

        glutSwapBuffers(); // do not modify this line..

    }

}


void NonPrintableKeys(int key, int x, int y)
{

    if ( key == GLUT_KEY_LEFT  )
    {
        if ( movement != 2 )     //can't move left on starting position and can't move left if snake is moving right
        {
    	   movement = 1 ;
        }
    } 
    
    else if ( key == GLUT_KEY_RIGHT )
    {
        if ( movement != 1 )    //cant move right if snake is moving left 
        {
    	   movement = 2 ;
        }
    }
    
    else if ( key == GLUT_KEY_UP ) 
    {
        if ( movement != 4 )    //cant move up if snake is moving down
        {
    	   movement = 3 ;
        }
    }
    
    else if ( key == GLUT_KEY_DOWN ) 
    {
        if ( movement != 3 )    //cant move down if snake is moving up
        {
    	   movement = 4 ;
        }
    }
    

    /* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
     * this function*/
     glutPostRedisplay();

}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */
void PrintableKeys(unsigned char key, int x, int y)
{
    if (key == KEY_ESC/* Escape key ASCII*/) 
    {
        exit(1); // exit the program when escape key is pressed.
    }
    if (key == 'R' || key=='r'/* Escape key ASCII*/) 
    {
        //exit(1); // exit the program when escape key is pressed.
    	//aswangle+=90;
    }
    else if (int(key) == 13)
    {

	}
    
    glutPostRedisplay();
}



/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */

void Timer(int m) 
{
    for ( int i=0 ; i<5 ; i++ )  //TO Increase Timer for Each food
    {
        timer[i] += 0.1 ;
    }

    macro_timer += 0.1 ;         // To Increase Timer for Macro Food

    collision_timer += 0.1 ;     // To stop exiting game in start and wait for 5s to move snake

    obs_timer += 0.1 ;           // To increase timer for obstacles

// implement your functionality here
	glutPostRedisplay();
// once again we tell the library to call our Timer function after next 1000/FPS
    glutTimerFunc(500.0 / FPS, Timer, 0);
}

/*
 * our gateway main function
 * */
int main(int argc, char*argv[])
{
	srand(time(NULL)) ;
    int width = 800, height = 600;                      // i have set my window size to be 800 x 600
    InitRandomizer();                                   // seed the random number generator...
    glutInit(&argc, argv);                              // initialize the graphics library...

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);       // we will be using color display mode
    glutInitWindowPosition(300, 100);                   // set the initial position of our window
    glutInitWindowSize(width, height);                  // set the size of our window
    glutCreateWindow("PF's Snake Game");                // set the title of our game window
    SetCanvasSize(width, height);                       // set the number of pixels...

    // Register your functions to the library,
    // you are telling the library names of function to call for different tasks.

    //glutDisplayFunc(display);                         // tell library which function to call for drawing Canvas.
    glutDisplayFunc(Display);                           // tell library which function to call for drawing Canvas.
    glutSpecialFunc(NonPrintableKeys);                  // tell library which function to call for non-printable ASCII characters
    glutKeyboardFunc(PrintableKeys);                    // tell library which function to call for printable ASCII characters

    // This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
    glutTimerFunc(5.0 / FPS, Timer, 0);

    // now handle the control to library and it will call our registered functions when
    // it deems necessary...
    glutMainLoop();
    return 1;
}

#endif /* Snake Game */

