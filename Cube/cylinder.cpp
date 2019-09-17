#include <GL/glut.h>
#include <cstdio>

GLuint startList;

//ע�����ص�
void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *estring;

	estring = gluErrorString(errorCode);
	//fprintf(stderr, "Quadric Error: %s\n", estring);
	exit(0);
}

void init(void)
{
	GLUquadricObj *qobj;
	GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	startList = glGenLists(4);
	//�������ζ���
	qobj = gluNewQuadric();
	//ע�������ص�
	gluQuadricCallback(qobj, GLU_ERROR,
		(void(__stdcall*)())errorCallback);
	//ָ�����ζ������Ⱦ����
	gluQuadricDrawStyle(qobj, GLU_FILL); //����ģʽ:���
	gluQuadricNormals(qobj, GLU_SMOOTH);//Ϊÿ����������һ�������Ҳ��ù⻬��ɫģʽ
	glNewList(startList, GL_COMPILE);
	//4�ֶ���ͼԪ��Ⱦ
	gluSphere(qobj, 0.75, 15, 10);//����
	glEndList();

	gluQuadricDrawStyle(qobj, GLU_FILL);//������
	gluQuadricNormals(qobj, GLU_FLAT);//��ɫ��ɫģʽ
	glNewList(startList + 1, GL_COMPILE);
	gluCylinder(qobj, 0.5, 0.3, 1.0, 15, 5);//̨׶��
	glEndList();

	gluQuadricDrawStyle(qobj, GLU_LINE);//�߿����
	gluQuadricNormals(qobj, GLU_NONE);//�����ɷ���(û�й���)
	glNewList(startList + 2, GL_COMPILE);
	gluDisk(qobj, 0.25, 1.0, 20, 4);//Բ��
	glEndList();

	gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);//�߿�ģʽ��ͬһƽ��������治��������֮��ı�
	gluQuadricNormals(qobj, GLU_NONE);
	glNewList(startList + 3, GL_COMPILE);
	gluPartialDisk(qobj, 0.0, 1.0, 20, 4, 0.0, 225.0);//����Բ��
	glEndList();

	gluDeleteQuadric(qobj);//ɾ�����ζ����ͷſռ�
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glTranslatef(-1.0, -1.0, 0.0);
	glCallList(startList);

	glShadeModel(GL_FLAT);
	glTranslatef(0.0, 2.0, 0.0);
	glPushMatrix();
	glRotatef(300.0, 1.0, 0.0, 0.0);
	glCallList(startList + 1);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(0.0, 1.0, 1.0);
	glTranslatef(2.0, -2.0, 0.0);
	glCallList(startList + 2);

	glColor3f(1.0, 1.0, 0.0);
	glTranslatef(0.0, 2.0, 0.0);
	glCallList(startList + 3);

	glPopMatrix();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-2.5, 2.5, -2.5*(GLfloat)h / (GLfloat)w,
			2.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-2.5*(GLfloat)w / (GLfloat)h,
			2.5*(GLfloat)w / (GLfloat)h, -2.5, 2.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}