#ifndef GL_UTILS
#define GL_UTILS
#define GLAD_GL_IMPLEMENTATION
#include<gl/gl.h>
#define GLFW_INCLUDE_NONE
#include<gl/glfw3.h>
#include<stdio.h>
#include<str_utils.h>
#include<nec.h>
GLFWwindow *window = 0;
#include<input.h>

#undef swap
#undef max
#undef min
#undef clamp
#define swap(a, b) (a += b, b = a - b, a -= b)
#define max(a, b) ((a) >= (b) ? (a) : (b))
#define min(a, b) ((a) <= (b) ? (a) : (b))
#define clamp(v, mi, ma) min(max(v, mi), ma)

double dt = 0.0, second = 0.0;
int fps = 0;

float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

void createWindow(const char* title, int width, int height)
{
    const char* description;
    if (!glfwInit())
    {
        glfwGetError(&description);
        printf("Error: %s\n", description);
        return;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwGetError(&description);
        printf("Error: %s\n", description);
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    glfwSwapInterval(0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, 0.0, 1.0);
}

void start( int(*loop)() )
{
    init_input();
    double last_time = 0.0;
    while(1)
    {
        glfwPollEvents();

        double new_time = glfwGetTime();
        double a = new_time - last_time;
        if(a < 1.0 / 60.0) continue;
        dt   = new_time - last_time;
        last_time = new_time;

        if(glfwWindowShouldClose(window) ||
        glfwGetKey(window, GLFW_KEY_ESCAPE)) break;

        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_POINTS);
        input_start();
        if(loop()) break;
        input_end();
        glEnd();
        glfwSwapBuffers(window);

        if(second <= 1.0)
        {
            second += dt;
            fps++;
        }
        else
        {
            // printf("FPS = %d\n", fps);
            fps = 0;
            second -= 1.0;
        }
    }
    glfwTerminate();
}

void pixel(int x, int y, unsigned char c)
{
    if(c == 0)      glColor3ub(0, 0, 0);       //BLACK
    else if(c == 1) glColor3ub(255, 255, 255); //WHITE
    else if(c == 2) glColor3ub(255, 0, 0);     //RED
    else if(c == 3) glColor3ub(0, 255, 0);     //GREEN
    else if(c == 4) glColor3ub(0, 0, 255);     //BLUE
    else if(c == 5) glColor3ub(255, 255, 0);   //YELLOW
    else if(c == 6) glColor3ub(0, 255, 255);   //PLAVA
    else if(c == 7) glColor3ub(255, 0, 255);   //PINK
    else if(c == 8) glColor3ub(200, 200, 0);   //DARK YELLOW
    else if(c == 9) glColor3ub(200, 0, 0);     //DARK RED

    // if(x < 0 || x >= 250) printf("ERROR X %d\n", x);
    // if(y < 0 || y >= 250) printf("ERROR Y %d\n", y);
    // if(x < 0 || x >= 250 || y < 0 || y >= 250) return;

    glVertex2i((x<<1)+1, (y<<1)+1);
}

void line(int x1, int y1, int x2, int y2, int c)
{
    int c1 = abs(x2 - x1);
    int c2 = abs(y2 - y1);
    float al = 0.0f;
    float inc = 1.0f;
    if(c1 > c2 && c1 != 0 || c2 >= c1 && c2 != 0) inc = 1.0f / (c1 > c2 ? c1 : c2);
    if(x1 > x2 && c1 > c2 || y1 > y2 && c2 >= c1)
    {
        x1 += x2;
        x2 = x1 - x2;
        x1 -= x2;
        y1 += y2;
        y2 = y1 - y2;
        y1 -= y2;
    }
    if(c1 > c2)
    {
        for(int i = 0; i <= c1; i++)
        {
            pixel(i + x1, y1 * (1 - al) + y2 * al, c);
            al += inc;
        }
    }
    else
    {
        for(int i = 0; i <= c2; i++)
        {
            pixel(x1 * (1 - al) + x2 * al, i + y1, c);
            al += inc;
        }
    }
}
#include<text.h>
#endif
