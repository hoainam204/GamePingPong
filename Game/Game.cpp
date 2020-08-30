
#include "Dependencies/Dependencies/glew/glew.h"
#include "Dependencies/Dependencies/freeglut/freeglut.h"

#include <stdio.h>
#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>

// keycodes
#define VK_W 0x57
#define VK_S 0x53


//Window size
int width = 1000;
int height = 500;
int interval = 1000 / 60;

// score
int score_left = 0;
int score_right = 0;

// Thuộc tính player
int racket_width = 10;
int racket_height = 80;
int racket_speed = 6;

// player bên trái
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;

// player bên phải
float racket_right_x = width - racket_width - 10;
float racket_right_y = 50;

// ball
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 8;
int ball_speed = 5;

// Vẽ player
void drawRect(float x, float y, float width, float height) 
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Vẽ bảng điểm
void drawText(float x, float y, std::string text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

//Ép kiểu dữ liệu từ Int qua String
std::string int2str(int x) {
    // converts int to string
    std::stringstream ss;
    ss << x;
    return ss.str();
}

void init(void)
{
    /* chọn màu để xóa nền (tức là sẽ phủ nền bằng màu này) */
    glClearColor(1.0, 1.0, 1.0, 1.0); /* màu đen */
   /* thiết lập các thông số cho view */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

//Main Draw
void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw player
    glColor3f(0.0, 0.0, 0.0);
    drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
    drawRect(racket_right_x, racket_right_y, racket_width, racket_height);

    // Vẽ bảng điểm
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(width / 2 - 10, height - 15, int2str(score_left) + ":" + int2str(score_right));

    // draw ball

    drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);

    glutSwapBuffers();
}

void keyboard() {
    // left racket
    if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
    if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;

    // right racket
    if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
    if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}

void vec2_norm(float& x, float& y) {
    // sets a vectors length to 1 (which means that x + y == 1)
    float length = sqrt((x * x) + (y * y));
    if (length != 0.0f) {
        length = 1.0f / length;
        x *= length;
        y *= length;
    }
}
// Ball Fly
void updateBall() {
    // fly a bit
    ball_pos_x += ball_dir_x * ball_speed;
    ball_pos_y += ball_dir_y * ball_speed;

    // hit by left racket?
    if (ball_pos_x < racket_left_x + racket_width &&
        ball_pos_x > racket_left_x &&
        ball_pos_y < racket_left_y + racket_height &&
        ball_pos_y > racket_left_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = t;
    }
    // hit by right racket?
    if (ball_pos_x > racket_right_x &&
        ball_pos_x < racket_right_x + racket_width &&
        ball_pos_y < racket_right_y + racket_height &&
        ball_pos_y > racket_right_y) {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
        float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = t;
    }
    // hit left wall?
    if (ball_pos_x < 0) {
        ++score_right;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = fabs(ball_dir_x); // force it to be positive
        ball_dir_y = 0;
    }

    // hit right wall?
    if (ball_pos_x > width) {
        ++score_left;
        ball_pos_x = width / 2;
        ball_pos_y = height / 2;
        ball_dir_x = -fabs(ball_dir_x); // force it to be negative
        ball_dir_y = 0;
    }

    // hit top wall?
    if (ball_pos_y > height) {
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
    }

    // hit bottom wall?
    if (ball_pos_y < 0) {
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
    }

    // make sure that length of dir stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);
}

//Main Update
void update(int value)
{
    //Nhận điều khiển từ bàn phím
    keyboard();

    // Bóng bay
    updateBall();

    // Gọi lại hàm update
    glutTimerFunc(interval, update, 0);

    glutPostRedisplay();
}

void enable2D(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); /* khởi tạo chế độ vẽ
   single buffer và hệ màu RGB */
    glutInitWindowSize(width, height);/* khởi tạo window kích thước 500 x 650 */
    glutCreateWindow("Pong Game");/* tên của window là ‘rectangle’ */

    /*Gọi Hàm*/
    init();
    glutDisplayFunc(Draw); /* thiết lập hàm vẽ là hàm Draw() */
    glutTimerFunc(interval, update, 0);

    //Setup màn hình ở dạng 2D và màu trắng
    enable2D(width, height);
    glColor3f(1.0f, 1.0f, 1.0f); // Màu trắng

    glutMainLoop(); /* bắt đầu chu trình lặp thể hiện vẽ */
    return 0;
}

