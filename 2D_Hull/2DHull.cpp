/*
Grant Nike
Nov 5th
6349302
COSC 3P98 Assignment #2
2D Convex Hull Application
*/

//For gl and glut
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//For vector of course
#include <vector>
//For cout,cin,cerr,endl
#include <iostream>
#include <limits>
//For Random
#include <cstdlib> 
//Used as parameter to random seed
#include <ctime>
//For max and min
#include <cmath>


//Menu options
enum{ MENU_RAND, MENU_PEEL, MENU_QUICK, MENU_ADD_100, MENU_RESET, MENU_QUIT};

//Points on the window
class point{
    public:
    GLint x,y;
    //Overload == operator to define if two points are equal
    bool operator ==(point p2){
        return x == p2.x && y == p2.y;
    }
    bool operator !=(point p2){
        return x != p2.x || y != p2.y;
    }
};

//Edges of the convex hull
class edge{
    public:
    point p1, p2;
    //Overload == operator to define if two edges are equal
    bool operator ==(edge e2){
        return p1 == e2.p1 && p2 == e2.p2 || p1 == e2.p2 && p2 == e2.p1;
    }
};

//the global structure
typedef struct {
    GLfloat screen_width = 1920.0/2;
    GLfloat screen_height = 1080.0/2;
    int numVertices = 100; // A default number of vertices
    bool pressedM = false; //Has the user started setting vertices with the mouse yet
    std::vector<point> vertices; // The list of vertices
    std::vector<edge> convex_hull; //The edges of the convex hull
} glob;
glob global;

//Checks if given point is already a vertex
bool inVertices(point p){
    bool result = false;

    for(int k=0;k<global.vertices.size();k++){
        if(global.vertices[k] == p) result = true;
    }

    return result;
}

//Randomly initializes n vertices
void random_init(int vertices){
    for(int k=0;k<vertices;k++){
        //Ensure all vertices are unique
        point p;
        do{
            p.x = random()%(int)(global.screen_width);
            p.y = random()%(int)(global.screen_height);
        }while(inVertices(p));
        global.vertices.push_back(p);
        //std::cout<<"x = "<<p.x<<" "<<"y = "<<p.y<<std::endl;
    }
}
//Draws the vertices and lines onto the screen
void draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    //Draw the vertices
    glBegin(GL_POINTS);
        for(int k=0;k<global.vertices.size();k++){
            point p = global.vertices[k];
            glPointSize(20);
            glColor3f(1.0,1.0,1.0);
            glVertex2i(p.x,p.y);
        }
    glEnd();
    //Draw the convex hull edges
    glBegin(GL_LINES);
        for(int k=0;k<global.convex_hull.size();k++){
            edge e = global.convex_hull[k];
            glColor3f(1.0,1.0,1.0);
            glVertex2i(e.p1.x,e.p1.y);
            glVertex2i(e.p2.x,e.p2.y);
        }
    //Clean up
    glEnd();
    glFlush();
    glutSwapBuffers();
}
//Generates n random vertices and draws them to the screen
void n_random(){
    int vertices;
    std::cout<<"Enter how many random vertices you want to generate: ";
    std::cin>>vertices;
    if(std::cin.fail()){
        std::cerr<<"Value must be an integer. Generating default value of 100 vertices"<<std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        random_init(global.numVertices);
    }
    else random_init(vertices);
    glutPostRedisplay();
}
// Draw the user selected vertices
void add_chosen(int button, int state, int x, int y){
   
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        point p;
        p.x = x;
        p.y = (int)((global.screen_height-y));
        if(!inVertices(p)) {global.vertices.push_back(p); std::cout<<"x = "<<p.x<<" "<<"y = "<<p.y<<std::endl;};
    }
    //Redraw the old vertices and the new one
    glutPostRedisplay();
}

//Helper for mouse input function, does nothing when not taking user mouse input
void do_nothing(int button, int state, int x, int y){

}

//Computes the signed fast distance between an edge and a point
GLint signed_fast_distance(edge e,point p){
    GLint a = e.p1.y - e.p2.y;
    GLint b = e.p2.x - e.p1.x;
    GLint c = (e.p1.x*e.p2.y)-(e.p1.y*e.p2.x);
    
    GLint d = (a*p.x) + (b*p.y) + c;
    return d;
}

point max_x(std::vector<point> vertices){
    point max = vertices[0];
    for(int i=0;i<vertices.size();i++){
        if(vertices[i].x > max.x){
            max = vertices[i];
        }
    }
    return max;
}

point min_x(std::vector<point> vertices){
    point min = vertices[0];
    for(int i=0;i<vertices.size();i++){
        if(vertices[i].x < min.x){
            min = vertices[i];
        }
    }
    return min;
}

//Helper function for quick hull, to figure out if a point is between the two edges of a line
bool isBetween(edge e, point p){
    return p.x>std::min(e.p1.x,e.p2.x) && p.x < std::max(e.p1.x,e.p2.x) && p.y>std::min(e.p1.y,e.p2.y) && p.y < std::max(e.p1.y,e.p2.y);
}

//Finds the convex hull of the existing points on the screen using the divide and conquer "quick hull" algorithm
void quick_Hull(std::vector<point> points, point P1,point P2){
    edge e;
    point pmax;
    e.p1 = P1;
    e.p2 = P2;
    int ind = -1;
    int maximum_distance = 0;
    pmax = points[0];
    //Find point with maximum distance value >= 0
    for(int i=0;i<points.size();i++){
        int dist = signed_fast_distance(e,points[i]);
        if(dist>maximum_distance){
            maximum_distance = dist;
            pmax = points[i];
            ind = i;
        }
    }
    //If no point exists with distance > 0 then find one between P1 and P2 with distance = 0
    if(maximum_distance == 0){
        for(int i=0;i<points.size();i++){
            int dist = signed_fast_distance(e,points[i]);
            if(dist == 0 && isBetween(e,points[i])){
                pmax = points[i];
            }
        }
    }
    //If there is no point with distance >= 0, add edge(P1,P2) to the hull
    if(ind == -1){
        global.convex_hull.push_back(e);
        return;
    }
    //Otherwise, recurse using pmax
    quick_Hull(points,P1,pmax);
    quick_Hull(points,pmax,P2);
}
//Initializes quick hull with points having a min and max x value, runs the quick hull, and draws it to the screen
void run_quick_hull(std::vector<point> points){
    if(points.size()>0){
        point P1 = min_x(points);
        point P2 = max_x(points);
        //Top
        quick_Hull(points,P1,P2);
        //Bottom
        quick_Hull(points,P2,P1);
        //Redraw screen to display hull
    }
    glutPostRedisplay();
}

void hull_peel(){
    //Make a copy of the global vertices called points
    std::vector<point> points;
    for(int i=0;i<global.vertices.size();i++){
        point p;
        p.x = global.vertices[i].x;
        p.y = global.vertices[i].y;
        points.push_back(p);
    }
    //Run quick hull on points not already in the hulls, until less than 3 points remain not in a hull
    while(points.size() > 3){
        run_quick_hull(points);
        //Removes points that are already in the hull for the next peel
        std::vector<point> p;
        for(int i=0;i<points.size();i++){
            bool add = true;
            for(int j=0;j<global.convex_hull.size();j++){
                if(points[i] == global.convex_hull[j].p1 || points[i] == global.convex_hull[j].p2){
                    add = false;
                }
            }
            if(add){
                p.push_back(points[i]);
            }
        }
        points = p;
    }
}
//Remove all vertices and convex hull edges from screen
void reset(){
    for(point p:global.vertices){
        global.vertices.pop_back();
    }
    for(edge e:global.convex_hull){
        global.convex_hull.pop_back();
    }
    glutPostRedisplay();
}

//Glut keyboard function to handle keyboard inputs
void keyboard(unsigned char key, int x, int y){
    switch (key){
        //Quit
        case 0x1B:
        case 'q':
        case 'Q':
            exit(0);
            break;
        case 'R':
        case 'r':
            n_random();
            break;
        case 'm':
        case 'M':
            if (!global.pressedM){glutMouseFunc(add_chosen); global.pressedM = true;}
            else {glutMouseFunc(do_nothing); global.pressedM = false;}
            break;
        case 'c':
        case 'C':
            run_quick_hull(global.vertices);
            break;
        case 'p':
        case 'P':
            hull_peel();
            break;
    }
}

//Defines what each menu function does
void menu_func(int value){
    switch(value){
        //Close the program
        case MENU_QUIT:
            exit(0);
            break;
        case MENU_RESET:
            reset();
            break;
        case MENU_RAND:
            n_random();
            break;
        case MENU_ADD_100:
            random_init(100);
            glutPostRedisplay();
            break;
        case MENU_QUICK:
            run_quick_hull(global.vertices);
            break;
        case MENU_PEEL:
            hull_peel();
            break;
    }
}

//Defines a menu accessed by right clicking the graphics window
void create_menu(){
    int main_menu = glutCreateMenu(&menu_func);
    glutAddMenuEntry("Generate n random vertices", MENU_RAND);
    glutAddMenuEntry("Add 100 random vertices", MENU_ADD_100);
    glutAddMenuEntry("Quick Hull",MENU_QUICK);
    glutAddMenuEntry("Hull Peel",MENU_PEEL);
    glutAddMenuEntry("Reset",MENU_RESET);
    glutAddMenuEntry("Quit", MENU_QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//Prints the key and its function to the terminal
void key_commands(){
    std::cout<<"R = Generate n random vertices"<<std::endl;
    std::cout<<"C = Convex Hull"<<std::endl;
    std::cout<<"C = Hull Peel"<<std::endl;
    std::cout<<"Q = Quit"<<std::endl;
}

main(int argc, char **argv){
    //Create random seed for random number generation
	srand(time(NULL));
    glutInit(&argc, argv);
    glutInitWindowSize(1920/2,1080/2);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutCreateWindow("2D Hull");
    
    glutDisplayFunc(draw);
    //glutIdleFunc(animate);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, global.screen_width, 0.0, global.screen_height); 
    glutKeyboardFunc(keyboard);
    key_commands();
    create_menu();
    //Runs the animate function every set number of ms
    //glutTimerFunc(global.busy_sleep, animate, 0);

    glutMainLoop();
    return(0);
}