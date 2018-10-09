#include <windows.h> //為使用MS Window 
#include <stdlib.h>
#include <GL/glut.h>  // GLUT,含glu.h及gl.h
//設定全域變數
char title[] = "3D Sierpinski-Triangle with light--s10459041";  //視窗名稱
typedef GLfloat point[3];
int refreshMills = 15;  //設定計數器間隔時間(毫秒)
static int teapot_rotation = 0;    //定義自轉角度
static int pyramid_rotation = 0;    //定義自轉角度
static GLfloat rx, ry, rz;   //定義自轉軸
static int pyramid_translatex = 0;   //定義x軸移動單位長度
static int pyramid_translatey = 0;   //定義y軸移動單位長度
static int pyramid_translatez = 0;   //定義z軸移動單位長度
static int pyramid_scalex = 1;   //定義缩放比例
static int pyramid_scaley = 1;
static int pyramid_scalez = 1;
int WinNumber;
/* initial tetrahedron */
point v[] = { { 0.0f, 0.0f, 1.0f },{ 0.0f, 0.942809f, -0.33333f },
{ -0.816497f, -0.471405f, -0.333333f },{ 0.816497f, -0.471405f, -0.333333f } }; //設定三角錐4個定點

GLfloat colors[4][3] = { { 1.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0 },
{ 0.0, 0.0, 1.0 },{ 1.0, 1.0, 0.0 } };  //設定三角錐顏色陣列

int n;

void triangle(GLfloat *va, GLfloat *vb, GLfloat *vc)  //數量法畫三角形
{
	glVertex3fv(va);
	glVertex3fv(vb);
	glVertex3fv(vc);
}

void triangles(point a, point b, point c)  //表皮法畫三角形
{
	glBegin(GL_POLYGON);
	glNormal3fv(a);
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
	glEnd();
}

void tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)  //數量法畫三角錐四面三角形並上色
{
	glColor3fv(colors[0]);
	triangle(a, b, c);
	glColor3fv(colors[1]);
	triangle(a, c, d);
	glColor3fv(colors[2]);
	triangle(a, d, b);
	glColor3fv(colors[3]);
	triangle(b, d, c);
}

void divide_triangle(point a, point b, point c, int m)  //表皮法用三頂點遞迴切割
{
	point v1, v2, v3;
	int j;
	if (m>0)
	{
		for (j = 0; j<3; j++) v1[j] = (a[j] + b[j]) / 2;
		for (j = 0; j<3; j++) v2[j] = (a[j] + c[j]) / 2;
		for (j = 0; j<3; j++) v3[j] = (b[j] + c[j]) / 2;
		divide_triangle(a, v1, v2, m - 1);
		divide_triangle(c, v2, v3, m - 1);
		divide_triangle(b, v3, v1, m - 1);
	}
	else(triangles(a, b, c));
}

void tetrahedron(int m)   //表皮法將三角錐四面切割完畢之後上色
{
	glColor3f(1.0, 0.0, 0.0);
	divide_triangle(v[0], v[1], v[2], m);
	glColor3f(0.0, 1.0, 0.0);
	divide_triangle(v[3], v[2], v[1], m);
	glColor3f(0.0, 0.0, 1.0);
	divide_triangle(v[0], v[3], v[1], m);
	glColor3f(1.0, 1.0, 0.0);
	divide_triangle(v[0], v[2], v[3], m);
}


void divide_tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)  //數量法用三角錐四個頂點定出6個中點遞迴切割,並將切割出的小三角錐四面上色
{
	GLfloat mid[6][3];
	int j;
	if (m>0)
	{
		/* compute six midpoints */
		for (j = 0; j<3; j++) mid[0][j] = (a[j] + b[j]) / 2;
		for (j = 0; j<3; j++) mid[1][j] = (a[j] + c[j]) / 2;
		for (j = 0; j<3; j++) mid[2][j] = (a[j] + d[j]) / 2;
		for (j = 0; j<3; j++) mid[3][j] = (b[j] + c[j]) / 2;
		for (j = 0; j<3; j++) mid[4][j] = (c[j] + d[j]) / 2;
		for (j = 0; j<3; j++) mid[5][j] = (b[j] + d[j]) / 2;
		divide_tetra(a, mid[0], mid[1], mid[2], m - 1);  //分割出4個三角錐
		divide_tetra(mid[0], b, mid[3], mid[5], m - 1);
		divide_tetra(mid[1], mid[3], c, mid[4], m - 1);
		divide_tetra(mid[2], mid[5], mid[4], d, m - 1);
	}
	else(tetra(a, b, c, d)); //遞迴結束繪製三角錐
}
static int menu_id;  //設定選單
static int submenu_id1, submenu_id2, submenu_id3, submenu_id4;
static int value = 0;
void menu(int num) {
	value = num;
	glutPostRedisplay();
}

void Rotate(int value)  //設定旋轉軸
{
	switch (value)
	{
	case 0:
		rx = 1.0f;  //X軸
		ry = 0.0f;
		rz = 0.0f;
		break;
	case 1:
		rx = 0.0f;   //Y軸
		ry = 1.0f;
		rz = 0.0f;
		break;
	case 2:
		rx = 0.0f;   //Z軸
		ry = 0.0f;
		rz = 1.0f;
		break;
	default:
		break;
	}
	glutPostRedisplay();  // 刷新窗口
}

void initGL() {  	//OpenGL Graphics初始化 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 設定背景為黑色
	glClearDepth(1.0f);                   // 將背景深度清除
	glEnable(GL_DEPTH_TEST);   // 啟動深度測試
	glDepthFunc(GL_LEQUAL);    // 深度測試設為LEQUAL
	glShadeModel(GL_SMOOTH);   // 啟動平滑著色
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // 投影設定-Nice perspective corrections
	glEnable(GL_COLOR_MATERIAL); // 啟動材質顏色
	glEnable(GL_NORMALIZE);
}

void display() {  //處理重繪事件
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除顏色緩衝區及深度緩衝區
	glMatrixMode(GL_MODELVIEW);     // 設定工作矩陣為MODELVIEW
	GLfloat tea_ambient[] = { 0.0f, 0.2f, 1.0f, 1.0f };  //材質設定
	glMaterialfv(GL_FRONT, GL_AMBIENT, tea_ambient);    //指定正面的環境反射和散射值
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0);  //镜面，反光

	GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f }; //設定環境光顏色(接近白色)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor); //設定光照模型

	GLfloat lightColor0[] = { 1.0f, 0.1f, 0.2f, 1.0f }; //設定light0為紅色光
	GLfloat lightPos0[] = { 0.0f, 3.5f, -1.0f, 0.0f }; //light0光源位置
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor0);  //設定light0為環境光
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	GLfloat lightColor1[] = { 0.1f, 1.0f, 1.0f, 1.0f }; //設定light1為藍色光
	GLfloat lightPos1[] = { 0.0f, 0.5f, -1.5f, 0.0f };//light1光源位置
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor1);  //設定light1為環境光
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

	glLoadIdentity();                  // 載入單位矩陣重設
	glTranslatef(0.0f, 0.0f, -6.0f);  // 將目標物移入螢幕視窗z軸負方向位置

	glTranslatef((GLfloat)pyramid_translatex, pyramid_translatey, pyramid_translatez);  //平移設定
	glScalef((GLfloat)pyramid_scalex, pyramid_scaley, pyramid_scalez);   // 缩放設定
	glRotatef((GLfloat)pyramid_rotation, rx, ry, rz);  // 設定旋轉

	if (value == 0) {
		glPushMatrix();
		glColor3f(1.0, 1.0, 0.0);
		glRotatef((GLfloat)teapot_rotation, 0.0f, 1.0f, 0.0f);
		glutSolidTeapot(0.5);          //畫茶壺
		glPopMatrix();
	}
	else if (value == 1) {
		glPushMatrix();
		tetrahedron(4);   //畫表皮法三角錐
		glPopMatrix();
	}
	else if (value == 2) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		divide_tetra(v[0], v[1], v[2], v[3], 4);  //畫數量法三角錐
		glEnd();
		glPopMatrix();
	}

	else if (value == 3) {
		glPushMatrix();
		tetrahedron(1);   //切割1次表皮法三角錐
		glPopMatrix();
	}
	else if (value == 4) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		divide_tetra(v[0], v[1], v[2], v[3], 1);  // 切割1次數量法三角錐
		glEnd();
		glPopMatrix();
	}
	else if (value == 5) {
		glPushMatrix();
		tetrahedron(2);   //切割2次表皮法三角錐
		glPopMatrix();
	}
	else if (value == 6) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);
		divide_tetra(v[0], v[1], v[2], v[3], 2);  // 切割1次數量法三角錐
		glEnd();
		glPopMatrix();
	}
	else if (value == 7) {
		glPushMatrix();
		tetrahedron(3);   //切割3次表皮法三角錐
		glPopMatrix();
	}
	else if (value == 8) {
		glPushMatrix();
		glBegin(GL_TRIANGLES);   //切割3次數量法三角錐
		divide_tetra(v[0], v[1], v[2], v[3], 3);
		glEnd();
		glPopMatrix();
	}

	glutSwapBuffers();  //前後frame buffers交換 (雙重緩衝)
	teapot_rotation += 2.0f;  //設定茶壺自轉
}

void ProcessMenu(int value)
{
	switch (value)
	{
	case 0:
		glEnable(GL_LIGHTING);  //開啟光照(Light0)
		glEnable(GL_LIGHT0);
		break;
	case 1:
		glDisable(GL_LIGHTING);  //關閉光照(Light0)
		glDisable(GL_LIGHT0);
		break;
	case 2:
		glEnable(GL_LIGHTING);    //開啟光照(Light1)
		glEnable(GL_LIGHT1);
		break;
	case 3:
		glDisable(GL_LIGHTING);  //關閉光照(Light1)
		glDisable(GL_LIGHT1);
		break;
	default:
		break;
	}
	glutPostRedisplay();  // 刷新窗口
}


void keyboard(unsigned char key, int x, int y)    //定義键盤按键操作
{
	switch (key) {
	case 'r':  //按r键則順時针自轉，每次旋轉角度为10度
		pyramid_rotation = (pyramid_rotation + 10) % 360;
		glutPostRedisplay();
		break;
	case 'R':  //按R键則逆時针自轉，每次旋轉角度10度
		pyramid_rotation = (pyramid_rotation - 10) % 360;
		glutPostRedisplay();
		break;
	case 'u':	//按u键沿Y軸正方向移動1單位
		pyramid_translatey = pyramid_translatey + 1;
		glutPostRedisplay();
		break;
	case 'd':	//按d键沿y軸負方向移動1單位
		pyramid_translatey = pyramid_translatey - 1;
		glutPostRedisplay();
		break;
	case 't':	//按t键沿X軸正方向移動1單位
		pyramid_translatex = pyramid_translatex + 1;
		glutPostRedisplay();
		break;
	case 'T':	//按T键沿X軸負方向移動1單位
		pyramid_translatex = pyramid_translatex - 1;
		glutPostRedisplay();
		break;
	case 'm':	//按m键沿Z軸正方向移動1單位
		pyramid_translatez = pyramid_translatez + 1;
		glutPostRedisplay();
		break;
	case 'b':	//按b键沿Z軸負方向移動1單位
		pyramid_translatez = pyramid_translatez - 1;
		glutPostRedisplay();
		break;
	case 's':		//按s键沿X,Y,Z軸增大
		pyramid_scalex = pyramid_scalex + 1;
		pyramid_scaley = pyramid_scaley + 1;
		pyramid_scalez = pyramid_scalez + 1;
		glutPostRedisplay();
		break;
	case 'S':	//按S键沿X,Y,Z軸缩小
		pyramid_scalex = pyramid_scalex - 1;
		pyramid_scaley = pyramid_scaley - 1;
		pyramid_scalez = pyramid_scalez - 1;
		glutPostRedisplay();
		break;
	case 'q':	//按q键離開
		glutDestroyWindow(WinNumber);
		exit(0);
		break;

	default:
		break;
	}
}

void timer(int value) {    //計數器時間到callback
	glutPostRedisplay();      //發出重繪要求啟動display()
	glutTimerFunc(refreshMills, timer, 0); //離下次呼叫隔refreshMills毫秒
}

void SetupRC(void)
{
	glShadeModel(GL_SMOOTH);  //平滑著色
	glFrontFace(GL_CW);   //順時針方向決定正面
}

void reshape(GLsizei width, GLsizei height) {  //視窗大小變動時 調整投影平面
	if (height == 0) height = 1;                // 避免分母為0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, width, height);//隨新視窗調整viewport
	glMatrixMode(GL_PROJECTION);  // 設定工作矩陣為PROJECTION
	glLoadIdentity();             //用單位矩陣重設
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);  //設定投影模式為perspective
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);   //定義視景轉換
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);            //初始化GLUT
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); //啟動雙緩衝模式
	glutInitWindowSize(1200, 750);   //設定視川大小(寬度及高度)
	glutInitWindowPosition(50, 50); //設定視窗左上角位置
	WinNumber = glutCreateWindow(title);          // 開啟標題視窗
	glutDisplayFunc(display);       //為重繪事件註冊callback處理器
	glutReshapeFunc(reshape);       //視窗調整大小事件註冊callback處理器
	initGL();                      // 初始化自訂的OpenGL 
	glutTimerFunc(0, timer, 0);     //第一次的立即呼叫
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	submenu_id1 = glutCreateMenu(menu);  //設定滑鼠選單
	glutAddMenuEntry("Surface Subdivision", 1);
	glutAddMenuEntry("Volume Subdivision", 2);
	submenu_id2 = glutCreateMenu(menu);
	glutAddMenuEntry("Subdivision Num:1-Surface ", 3);
	glutAddMenuEntry("Subdivision Num:1-Volume", 4);
	glutAddMenuEntry("Subdivision Num:2-Surface", 5);
	glutAddMenuEntry("Subdivision Num:2-Volume", 6);
	glutAddMenuEntry("Subdivision Num:3-Surface", 7);
	glutAddMenuEntry("Subdivision Num:3-Volume", 8);
	submenu_id3 = glutCreateMenu(Rotate);
	glutAddMenuEntry("X_Asix", 0);
	glutAddMenuEntry("Y_Asix", 1);
	glutAddMenuEntry("Z_Asix", 2);
	submenu_id4 = glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Light0 On", 0);
	glutAddMenuEntry("Light0 Off", 1);
	glutAddMenuEntry("Light1 On", 2);
	glutAddMenuEntry("Light1 Off", 3);
	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("TeaPot", 0);
	glutAddSubMenu("Sierpinski Gasket", submenu_id1);
	glutAddSubMenu("Subdivision Number", submenu_id2);
	glutAddSubMenu("Rotation", submenu_id3);
	glutAddSubMenu("Light", submenu_id4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);// 創建右键選单

	SetupRC();
	glutKeyboardFunc(keyboard); //接收键盘操作
	glutMainLoop();  //進入無窮迴圈
	return 0;
}

