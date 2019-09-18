#include "header.h"


GLfloat  xrot, yrot, zrot;
GLfloat xlen, ylen, zlen, bxlen, bzlen;
string input_path, save_path;
GLuint texture[10];
GLint  Slice, split, frame_num;
bool gen_traj,use_main;
GLfloat fx, fy, u0, v0;
GLfloat posx, posy, posz;
vector<float> vxlen, vylen, vzlen, vbxlen, vbzlen;
json class2id;

struct point {
	float x, y, z;
};

vector<vector<point>> trajectory[18];

int createDirectory(const std::string &directoryPath)
{
	unsigned int dirPathLen = directoryPath.length();
	char tmpDirPath[256] = { 0 };
	for (unsigned int i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) != 0)
			{
				int ret = _mkdir(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}

void saveSceneImage(const string fileName)
{
	cout << fileName << endl;
	GLint ViewPort[4];
	glGetIntegerv(GL_VIEWPORT, ViewPort);
	GLsizei ColorChannel = 4;
	GLsizei bufferSize = ViewPort[2] * ViewPort[3] * sizeof(GLubyte)*ColorChannel;
	GLubyte * ImgData = (GLubyte*)malloc(bufferSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(ViewPort[0], ViewPort[1], ViewPort[2], ViewPort[3], GL_BGRA_EXT, GL_UNSIGNED_BYTE, ImgData);

	// stbi save image
	//stbi__vertical_flip(ImgData, ViewPort[2], ViewPort[3], ColorChannel * sizeof(stbi_uc));
	//stbi_write_png(fileName.c_str(),ViewPort[2], ViewPort[3],ColorChannel, ImgData, 0);
	//free(ImgData);

	//freeImage save image
	FIBITMAP* dst = FreeImage_ConvertFromRawBits(ImgData, ViewPort[2], ViewPort[3], 4 * ViewPort[2], 32,
		FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE);
	free(ImgData);
	FreeImage_Save(FIF_PNG, dst, fileName.c_str());
	FreeImage_Unload(dst);
}


void StbiLoadGLTexture(GLuint *texture, char *png_file_name, int texture_id) {
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(png_file_name, &width, &height, &nrChannels, 0);
	if (data)
	{
		glGenTextures(texture_id, texture); 
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线形滤波
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线形滤波
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data); 
}

void material() {
	GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };  //定义材质的环境光颜色，
	GLfloat mat_diffuse[] = { 0.0f, 0.0f, 0.0f, 1.0f };  //定义材质的漫反射光颜色，
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };   //定义材质的镜面反射光颜色，
	GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };   //定义材质的辐射光颜色，为0
	GLfloat mat_shininess = 30.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}
//前后
void DrawFlat() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存
	glLoadIdentity();         // 重置当前的模型观察矩阵
	if (use_main)
		gluLookAt(16.8766, -7.7111, -212.7446, 16.9178, -7.5148, -211.7650, 0.0163, -0.9805, 0.1958);
	else
		gluLookAt(-5.1259, -8.2751, -196.5466, -4.5663, -8.0399, -195.7519, 0.2259, -0.9659, 0.1268);

	glTranslatef(posx, posy, -posz);         // 移入屏幕 5 个单位
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);         // 绕X轴旋转
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);         // 绕Y轴旋转
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);         // 绕Z轴旋转

	glBindTexture(GL_TEXTURE_2D, texture[0]);      // 选择纹理
	glBegin(GL_QUADS); //前面
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-xlen / 2.0, -ylen / 2.0, zlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(xlen / 2.0, -ylen / 2.0, zlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);													 // 后面
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-xlen / 2.0, -ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(xlen / 2.0, -ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左下
	glEnd();

	glFlush();
}

//前后 顶底 左右
void DrawCube(void)         // 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存

	glLoadIdentity();         // 重置当前的模型观察矩阵
	if (use_main)
		gluLookAt(16.8766, -7.7111, -212.7446, 16.9178, -7.5148, -211.7650, 0.0163, -0.9805, 0.1958);
	else
		gluLookAt(-5.1259, -8.2751, -196.5466, -4.5663, -8.0399, -195.7519, 0.2259, -0.9659, 0.1268);

	glTranslatef(posx, posy, -posz);         // 移入屏幕 5 个单位
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);         // 绕X轴旋转
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);         // 绕Y轴旋转
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);         // 绕Z轴旋转

	glBindTexture(GL_TEXTURE_2D, texture[0]);      // 选择纹理
	glBegin(GL_QUADS); //前面
	glNormal3f(0, 0, 1.0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-bxlen/2.0, -ylen/2.0, bzlen/2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左上
	glEnd();

	GLint nSlice = 10;
	GLfloat t = 0, dt = 1.0 / nSlice;
	glBegin(GL_QUAD_STRIP);
	for (int j = 0; j <= nSlice; ++j)
	{
		glNormal3f(0, 0, 2 * (-abs(t - 0.5) + 0.5));
		glTexCoord2f(t , 1.0);
		glVertex3f(-xlen / 2 + t * xlen, ylen / 2.0, zlen / 2.0);
		glTexCoord2f(t , 0.0);
		glVertex3f(-xlen / 2 + t * xlen, -ylen / 2.0, zlen / 2.0);
		t = t + dt;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);													 // 后面
	glNormal3f(0, 0, -1);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen/2.0, -ylen/2.0, -bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-xlen/2.0, ylen/2.0, -zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(xlen/2.0, ylen/2.0, -zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen/2.0, -ylen/2.0, -bzlen / 2.0); // 纹理和四边形的左下
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);	// 顶面
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen/2.0, ylen/2.0, -zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-xlen/2.0, ylen/2.0, zlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(xlen/2.0, ylen/2.0, zlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen/2.0, ylen/2.0, -zlen / 2.0); // 纹理和四边形的右上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);															 // 底面
	glNormal3f(0, -1, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-bxlen/2.0, -ylen/2.0, -bzlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(bxlen/2.0, -ylen/2.0, -bzlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen/2.0, -ylen/2.0, bzlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen/2.0, -ylen/2.0, bzlen / 2.0); // 纹理和四边形的右下
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_QUADS);// 左面
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_QUADS);	// 右面
	glNormal3f(1, 0, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的左下
	glEnd();

	glFlush(); 
}


//顶 侧 底
void DrawCylinder() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存
	
	glLoadIdentity();         // 重置当前的模型观察矩阵
	if (use_main)
		gluLookAt(16.8766, -7.7111, -212.7446, 16.9178, -7.5148, -211.7650, 0.0163, -0.9805, 0.1958);
	else
		gluLookAt(-5.1259, -8.2751, -196.5466, -4.5663, -8.0399, -195.7519, 0.2259, -0.9659, 0.1268);

	glPushMatrix();
	glTranslatef(posx, posy, -posz);         // 移入屏幕 
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);         // 绕X轴旋转
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);         // 绕Y轴旋转
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);         // 绕Z轴旋转

	int nSlice = 32;
	GLdouble PI = 3.1415926, dt = 2 * PI / nSlice;
	GLdouble t;

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_POLYGON);	t = 0;
	for (int j = 0; j <= nSlice; ++j) {
		glTexCoord2f(0.5 + cos(t) / 2.0, 0.5 + sin(t) / 2.0);
		glVertex3f(xlen / 2.0*cos(t), ylen / 2.0*1.0, zlen / 2.0*-sin(t));
		t = t + dt;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUAD_STRIP);
	for (int j = 0; j <= nSlice; ++j)
	{
		glTexCoord2f(t / (2 * PI), 1.0);
		glVertex3f(xlen / 2.0*cos(t), ylen / 2.0*1.0, zlen / 2.0*-sin(t));
		glTexCoord2f(t / (2 * PI), 0.0);
		glVertex3f(bxlen / 2.0*cos(t), ylen / 2.0*-1.0, bzlen / 2.0*-sin(t));
		t = t + dt;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);	t = 0;
	for (int j = 0; j <= nSlice; ++j) {
		glTexCoord2f(0.5 + cos(t) / 2.0, 0.5 + sin(t) / 2.0);
		glVertex3f(bxlen / 2.0*cos(t), ylen / 2.0*-1.0, bzlen / 2.0*-sin(t));
		t = t - dt;
	}
	glEnd();
	glPopMatrix();

	glFlush();
}

void DrawGyrator() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存

	glLoadIdentity();         // 重置当前的模型观察矩阵
	if (use_main)
		gluLookAt(16.8766, -7.7111, -212.7446, 16.9178, -7.5148, -211.7650, 0.0163, -0.9805, 0.1958);
	else
		gluLookAt(-5.1259, -8.2751, -196.5466, -4.5663, -8.0399, -195.7519, 0.2259, -0.9659, 0.1268);
	material();
	GLfloat mid = 0;
	//移动至景深，旋转绘图，上移至中心
	for (auto y : vylen) mid += y;
	mid /= 2;
	glTranslatef(posx, posy, -posz);         // 移入屏幕 
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);         // 绕X轴旋转
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);         // 绕Y轴旋转
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);         // 绕Z轴旋转
	glTranslatef(0.0f, mid, 0.0f);         //向上移动至瓶子中心
	GLfloat top = 0;
	int nSlice = 32;
	GLdouble PI = 3.1415926, dt = 2 * PI / nSlice;
	GLdouble t;

	//top
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	t = 0;
	for (int j = 0; j <= nSlice; ++j) {
		glTexCoord2f(0.5 + cos(t) / 2.0, 0.5 + sin(t) / 2.0);
		glVertex3f(vxlen[0] / 2.0*cos(t), top, vzlen[0] / 2.0*-sin(t));
		t = t + dt;
	}
	glEnd();

	//side
	for (int i = 1; i < 1 + split; i++) {
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= nSlice; ++j)
		{
			glNormal3f(cos(t), 0, -sin(t));
			glTexCoord2f(t / (2 * PI), 1.0);
			glVertex3f(vxlen[i-1] / 2.0*cos(t), top, vzlen[i-1] / 2.0*-sin(t));
			glTexCoord2f(t / (2 * PI), 0.0);
			glVertex3f(vbxlen[i-1] / 2.0*cos(t), top - vylen[i-1], vbzlen[i-1] / 2.0*-sin(t));
			t = t + dt;
		}
		top -= vylen[i - 1];
		glEnd();
	}

	//bottom
	glBindTexture(GL_TEXTURE_2D, texture[1 + split]);
	glBegin(GL_POLYGON);
	t = 0;
	for (int j = 0; j <= nSlice; ++j) {
		glTexCoord2f(0.5 + cos(t) / 2.0, 0.5 + sin(t) / 2.0);
		glVertex3f(vbxlen[split-1] / 2.0*cos(t), top, vbzlen[split-1] / 2.0*-sin(t));
		t = t - dt;
	}
	glEnd();

	glFlush();
}

void DrawTusi(void)         // 从这里开始进行所有的绘制
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存

	glLoadIdentity();         // 重置当前的模型观察矩阵
	if (use_main)
		gluLookAt(16.8766, -7.7111, -212.7446, 16.9178, -7.5148, -211.7650, 0.0163, -0.9805, 0.1958);
	else
		gluLookAt(-5.1259, -8.2751, -196.5466, -4.5663, -8.0399, -195.7519, 0.2259, -0.9659, 0.1268);

	glTranslatef(posx, posy, -posz);         // 移入屏幕 5 个单位
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);         // 绕X轴旋转
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);         // 绕Y轴旋转
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);         // 绕Z轴旋转

	glBindTexture(GL_TEXTURE_2D, texture[0]);      // 选择纹理
	glBegin(GL_QUADS); //前面
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);													 // 后面
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的左下
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);	// 顶面
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0-3, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-xlen / 2.0-3, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(xlen / 2.0+3, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0+3, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);	// 底面
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-bxlen / 2.0-3, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(bxlen / 2.0+3, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen / 2.0+3, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen / 2.0-3, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的右下
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_QUADS);// 左面
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的左下
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的左上
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_QUADS);	// 右面
	glTexCoord2f(1.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, -bzlen / 2.0); // 纹理和四边形的右下
	glTexCoord2f(1.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, -zlen / 2.0); // 纹理和四边形的右上
	glTexCoord2f(0.0f, 1.0f); glVertex3f(xlen / 2.0, ylen / 2.0, zlen / 2.0); // 纹理和四边形的左上
	glTexCoord2f(0.0f, 0.0f); glVertex3f(bxlen / 2.0, -ylen / 2.0, bzlen / 2.0); // 纹理和四边形的左下
	glEnd();

	glFlush();
}

vector<string> FindFile(std::string path) {
	long Handle;
	struct _finddata_t FileInfo;
	vector<string> res;
	if ((Handle = _findfirst(path.c_str(), &FileInfo)) == -1L)
		printf("没有找到匹配的项目\n");
	else
	{
		if (FileInfo.name[0] != '.')
			res.push_back(path.substr(0, path.find("*")) + FileInfo.name);
		while (_findnext(Handle, &FileInfo) == 0)
			if (FileInfo.name[0] != '.')
				res.push_back(path.substr(0, path.find("*")) + FileInfo.name);
		_findclose(Handle);
	}
	return res;
}


void init(string input_folder, vector<string> texs)
{
	char* path = new char[100];
	for (int i = 0; i < texs.size(); i++) {
		strcpy(path, (input_folder + "\\" + texs[i]).c_str());
		StbiLoadGLTexture(&texture[i], path , 1);            //载入纹理贴图
	}

	glClearColor(1.0, 1.0, 1.0, 0.0);            //清理颜色，为黑色，（也可认为是背景颜色）
	//glCullFace(GL_BACK);                        //背面裁剪(背面不可见)
	//glEnable(GL_CULL_FACE);                        //启用裁剪
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//对齐 防止图片偏斜
	glShadeModel(GL_SMOOTH);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0);
	//指定环境模式为：纹理单位颜色与几何图形颜色相乘。
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_COLOR_MATERIAL);//启用颜色追踪  
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0f }; //RGBA模式的环境光，全白光
	//GLfloat light_diffuse[] = { 0.0, 0.0, 0.0, 1.0f }; //RGBA模式的漫反射光，全白光
	//GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0f };  //RGBA模式下的镜面光 ，全白光
	//GLfloat light_position[] = { posx, posy, -posz+1000, 0.0f }; //光源的位置
	//GLfloat light_direction[] = { 0.0,0.0,1.0 };//光源的位置
	//GLfloat attenuation = 0.1;

	//GLfloat  whiteLight[] = { 0.5, 0.5, 0.5, 1.0f };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
	//glLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, &attenuation);
	//glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, &attenuation);

	//glEnable(GL_LIGHT0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void reshape(GLsizei width, GLsizei height)
{
	//这里小说明一下：矩阵模式是不同的，他们各自有一个矩阵。投影相关
	//只能用投影矩阵。(只是目前情况下哦，等我学多了可能就知道为什么了。)
	
	glMatrixMode(GL_PROJECTION);    //设置矩阵模式为投影变换矩阵，
	glLoadIdentity();                //变为单位矩阵
	GLfloat Near = 10.0, Far = 2160.0; //使用毫米为单位
	GLfloat left=-u0*Near/fx, right=(width-u0)*Near/fx, bottom=(v0-height)*Near/fy, top=v0*Near/fy;
	glFrustum(left, right, bottom, top, Near, Far);
	glViewport(0, 0, width, height);        //设置视口
	glMatrixMode(GL_MODELVIEW);        //设置矩阵模式为视图矩阵(模型)
	glLoadIdentity();                //变为单位矩阵
}



void generate_image_for_one_folder(json Json, string input_path, string save_path, string kind) {
	GLint window = glutCreateWindow("OpenGL纹理贴图");
	glutReshapeFunc(reshape);                //绘制图形时的回调


	int type = Json["type"];
	switch (type)
	{
	case 0:
		xlen = Json["xlen"]; ylen = Json["ylen"]; zlen = Json["zlen"];
		init(input_path, Json["textures"]);  //初始化资源,这里一定要在创建窗口以后，不然会无效。
		glutDisplayFunc(DrawFlat);
		break;
	case 1:
		xlen = Json["xlen"]; ylen = Json["ylen"]; zlen = Json["zlen"];
		bxlen = Json["bxlen"]; bzlen = Json["bzlen"];
		init(input_path, Json["textures"]);  //初始化资源,这里一定要在创建窗口以后，不然会无效。
		glutDisplayFunc(DrawCube);
		break;
	case 2:
		xlen = Json["xlen"]; ylen = Json["ylen"]; zlen = Json["zlen"];
		bxlen = Json["bxlen"]; bzlen = Json["bzlen"];
		init(input_path, Json["textures"]);  //初始化资源,这里一定要在创建窗口以后，不然会无效。
		glutDisplayFunc(DrawCylinder);
		break;
	case 3:
		vxlen = Json["xlen"].get<std::vector<GLfloat>>(); vylen = Json["ylen"].get<std::vector<GLfloat>>(); vzlen = Json["zlen"].get<std::vector<GLfloat>>();
		vbxlen = Json["bxlen"].get<std::vector<GLfloat>>(); vbzlen = Json["bzlen"].get<std::vector<GLfloat>>(); split = Json["split"];
		init(input_path, Json["textures"]);  //初始化资源,这里一定要在创建窗口以后，不然会无效。
		glutDisplayFunc(DrawGyrator);
		break;
	case 4:
		xlen = Json["xlen"]; ylen = Json["ylen"]; zlen = Json["zlen"];
		bxlen = Json["bxlen"]; bzlen = Json["bzlen"];
		init(input_path, Json["textures"]);  //初始化资源,这里一定要在创建窗口以后，不然会无效。
		glutDisplayFunc(DrawTusi);
		break;
	default:
		cout << "not supported type" << endl;
		break;
	}
	GLint index = 0; GLfloat angle_ps = 360.0 / Slice;
	if (gen_traj) {
		if (kind.find("_") != string::npos) {
			kind = kind.substr(0, kind.find("_"));
		}
		int k = class2id[kind];
		int size = trajectory[k].size();
		vector<point> T = trajectory[k][0];
		xrot = 0; yrot = 180; zrot = 180;
		while (index < T.size()) {
			GLint temp = index;
			posx = T[index].x; posy = T[index].y; posz = T[index].z;
			xrot += disangle(gen); //yrot += disangle(gen)*angle; zrot += disangle(gen)*angle;
			glutMainLoopEvent();
			saveSceneImage(save_path + "\\" + to_string(index) + ".png");
			glutPostRedisplay();  //重画，相当于重新调用Display(),改编后的变量得以传给绘制函数
			index++;
		}
		glutDestroyWindow(window);
	}
	else {
		while (index < Slice*Slice) {
			GLint temp = index;

			xrot = (temp % Slice)*angle_ps; temp /= Slice;
			yrot = (temp % Slice)*angle_ps; temp /= Slice;
			// zrot = (temp % Slice)*angle_ps;
			glutMainLoopEvent();
			if (type == 0 && (xrot == 72 || xrot == 108 || xrot == 252 || xrot == 288) && (yrot == 72 || yrot == 108 || yrot == 252 || yrot == 288)) {
				cout << "not ok"<<endl;
			}
			else
				saveSceneImage(save_path + "\\" + to_string(int(xrot)) + "_" + to_string(int(yrot)) + ".png");
			glutPostRedisplay();  //重画，相当于重新调用Display(),改编后的变量得以传给绘制函数
			index++;
		}
		glutDestroyWindow(window);
	}
	
}

void process_folder(string input_folder, string save_folder, string kind="") {
	cout << input_folder << endl;

	const char *save_folder_c = save_folder.c_str();
	if (_access(save_folder_c, 6) == -1)
		_mkdir(save_folder_c);

	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName;

	hFile = _findfirst(pathName.assign(input_folder).append("\\*").c_str(), &fileInfo);

	if (hFile == -1) {
		cout << "Failed to find first file while process folder!\n";
		return;
	}

	do {
		string filename = fileInfo.name;
		if (strcmp(fileInfo.name, ".") == 0 || strcmp(fileInfo.name, "..") == 0)
			continue;

		if (fileInfo.attrib & _A_SUBDIR) {
			process_folder(input_folder + "\\" + filename, save_folder + "\\" + filename, filename);
		}
		else {
			string postfix = filename.substr(filename.length() - 4, filename.length());	//后缀
			if (postfix.compare("json") == 0 ) {
				fstream fin(input_folder+"\\"+filename); json Json;
				fin >> Json; fin.close();
				generate_image_for_one_folder(Json, input_folder, save_folder, kind);
			}
		}

	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);    // 关闭搜索句柄

}

void get_trajectory(string path="G:/smart_shelf/数据集/trajectory") {
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName;

	hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo);

	if (hFile == -1) {
		cout << "Failed to find first file while process folder!\n";
		return;
	}

	do {
		string filename = fileInfo.name;
		if (strcmp(fileInfo.name, ".") == 0 || strcmp(fileInfo.name, "..") == 0)
			continue;

		string postfix = filename.substr(filename.length() - 3, filename.length());	//后缀
		int kind = atoi(filename.substr(0, filename.find('_')).c_str());
		// cout << filename <<" " << filename.substr(0, filename.find('_')) << endl;
		if (postfix.compare("txt") == 0) {
			point x; vector<point> temp;
			fstream fin(path + "\\" + filename);
			while (fin >> x.x >> x.y >> x.z) {
				temp.push_back(x);
			}
			trajectory[kind].push_back(temp);
		}

	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);    // 关闭搜索句柄
}

int main(int argc, char *argv[])
{
	/*if (argc < 4) {
		puts("please input fllowing parameters");
		cout << "	--input_path" << endl;
		cout << "	--save_path" << endl;
		cout << "	--slice" << endl;
		return 0;
	}
	input_path = argv[1];
	save_path = argv[2];
	Slice = atoi(argv[3]);*/

	get_trajectory();
	fstream fin("class2id_new.json");
	fin >> class2id;
	gen_traj = true;
	use_main = false;
	input_path = "G:\\sample_generate_new\\ordinary_template";
	save_path = "G:\\sample_generate_new\\traj_ordinary_sub";
	posx = 10, posy = 0, posz = 170; //use for image, not trajectory
	Slice = 10;

	if (!use_main)
		// sub
		fx = 719.746814929738, fy = 717.772081837273, u0 = 749.707739198625, v0 = 363.640069007236;
	else
		// main
		fx = 930.882715657638, fy = 930.127415304179, u0 = 719.245819050168, v0 = 404.364554839123;

	glutInit(&argc, argv);  //固定格式
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA); //注意这里
	glutInitWindowSize(1280, 800);    //显示框的大小
	glutInitWindowPosition(100, 100); //确定显示框左上角的位置
	process_folder(input_path, save_path);

	
	return 0;

}