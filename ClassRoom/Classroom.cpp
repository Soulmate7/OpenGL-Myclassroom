#include<gl/glut.h>
#include<windows.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
GLint	WinWidth;
GLint	WinHeight;

//定义视点结构
typedef struct EyePoint
{
	GLfloat	x;
	GLfloat y;
	GLfloat z;
}EyePoint;
EyePoint myEye;
EyePoint vPoint;
EyePoint up;
GLfloat vAngle = 0;
//载入位图作为纹理的相关函数
#define BMP_Header_Length 54

//判断一个数是否是2的整数次方
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// 打开文件，如果失败，返回    
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// 读取文件中图象的宽度和高度    
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// 计算每行像素所占字节数，并根据此数据计算总像素字节数    
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// 根据总像素字节数分配内存    
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// 读取像素数据    
	fread(pixels, total_bytes, 1, pFile);

	// 对就旧版本的兼容，如果图象的宽度和高度不是2的整数次方，则需要进行缩放    
	// 若图像宽高超过了OpenGL规定的最大值，也缩放    
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (power_of_two(width)
			|| power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形    
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// 计算每行需要的字节数和总字节数    
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// 分配内存    
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// 进行像素缩放    
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height    
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// 分配一个新的纹理编号    
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复    
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定    
	free(pixels);
	return texture_ID;
}
//定义各个纹理对象的名称
GLuint texblackboard, texwindow, texdesk, texsound;
GLuint texceiling, texdoor, texfloor, texbackwall, texpole;
GLuint texairfro, texairback, texhighland, texsdesk, texclock, texcsc,texcuc,texlmy;

//绘制教室这个大场景
void drawscence()
{


	/*glTexCoord2f绘制图形时指定纹理的坐标，第一个是X轴坐标，0.0是纹理的左侧，0.5是纹理的中点，1.0是纹理的右侧，
	第二个是Y轴坐标，0.0是纹理的底部，0.5是纹理的中点，1.0是纹理的顶部，
	为了将纹理正确的映射到四边形上，您必须将纹理的右上角映射到四边形的右上角，纹理的左上角映射到四边形的左上角，
	纹理的右下角映射到四边形的右下角，纹理的左下角映射到四边形的左下角，纹理的左上坐标是X：0.0，Y：1.0f，四边形的左上顶点是X：-1.0，Y：1.0。*/
	
	
	glEnable(GL_TEXTURE_2D);//开启2D纹理功能
	//天花板
	glBindTexture(GL_TEXTURE_2D, texceiling);//绑定纹理
	glBegin(GL_QUADS);//画四边形
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 30.0f, 30.0f);//左下
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);//左上
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);//右上
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 30.0f, 30.0f);//右下
	glEnd();


	//绘制地板
	glBindTexture(GL_TEXTURE_2D, texfloor);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();

	//绘制左边墙
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glEnd();

	//绘制左边窗户
	glBindTexture(GL_TEXTURE_2D, texwindow);
	for (int n = 0; n <= 1; n++)
	{
		glBegin(GL_QUADS);
		glNormal3f(1.0, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-39.9, 10, -8 + n * 18);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-39.9, 20, -8 + n * 18);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-39.9, 20, -18 + n * 18);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-39.9, 10, -18 + n * 18);
		glEnd();
	}

	//绘制右边墙
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glEnd();

	//绘制右边窗户
	glBindTexture(GL_TEXTURE_2D, texwindow);
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0f, 0.0f);	                  //用于定义法线向量
	glTexCoord2f(1.0f, 0.0f); glVertex3f(39.5, 10, 10);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.5, 20, 10);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.5, 20, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.5, 10, 0);
	glEnd();

	//绘制后边墙
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();

	//CUC
	glBindTexture(GL_TEXTURE_2D, texcuc);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.8f, 15.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-4.8f, 20.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(4.8f, 20.0f, 30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(4.8f, 15.0f, 30.0f);
	glEnd();

	//CSC
	glBindTexture(GL_TEXTURE_2D, texcsc);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-15.0f, 10.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-15.0f, 15.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(15.0f, 15.0f, 30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(15.0f, 10.0f, 30.0f);
	glEnd();

	//绘制前边墙
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glEnd();

	//画钟
	glBindTexture(GL_TEXTURE_2D, texclock);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(23.0f, 18.0f, -29.8f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(23.0f, 20.0f, -29.8f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(25.0f, 20.0f, -29.8f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(25.0f, 18.0f, -29.8f);
	glEnd();

	//绘制空调
	glBindTexture(GL_TEXTURE_2D, texairfro);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(33.0f, 0.0f, -26.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(33.0f, 15.0f, -26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(37.0f, 15.0f, -26.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(37.0f, 0.0f, -26.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texairback);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(33.0f, 0.0f, -26.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(33.0f, 15.0f, -26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(33.0f, 15.0f, -29.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(33.0f, 0.0f, -29.0f);
	glEnd();

	//绘制教室两边石柱前边两根
	glBindTexture(GL_TEXTURE_2D, texpole);
	for (int i = 0; i <= 1; i++)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		//glColor3f(255, 255, 255);
		//石柱上表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0f + i * 78, 30.0f, -4.0f);
		glEnd();
		//石柱前表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 0.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 78, 0.0f, -4.0f);
		glEnd();
		//石柱后表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 0.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 78, 0.0f, -6.0f);
		glEnd();
		//石柱右表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0 + i * 76, 0.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0 + i * 76, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 76, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 76, 0.0f, -6.0f);
		glEnd();
	}
	//绘制教室两边石柱，后边两根
	for (int j = 0; j <= 1; j++)
	{

		glColor3f(1.0f, 1.0f, 1.0f);
		//glColor3f(255, 255, 255);
		//石柱上表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + j * 78, 30.0f, 14.0f);
		glEnd();
		//石柱前表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 0.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 78, 0.0f, 14.0f);
		glEnd();
		//石柱后表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 0.0f, 12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 78, 0.0f, 12.0f);
		glEnd();
		//右表面
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //用于定义法线向量
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0 + j * 76, 0.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0 + j * 76, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 76, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 76, 0.0f, 12.0f);
		glEnd();
	}

	//绘制黑板
	glBindTexture(GL_TEXTURE_2D, texblackboard);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0, 8.0f, -29.9f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0, 8.0f, -29.9f);
	glEnd();

	//lmy
	glBindTexture(GL_TEXTURE_2D, texlmy);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0, 20.0f, -29.9f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0, 25.0f, -29.9f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0, 25.0f, -29.9f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0, 20.0f, -29.9f);
	glEnd();
	
	//绘制教室前边一块高地并贴纹理
	glBindTexture(GL_TEXTURE_2D, texhighland);
	//贴上面
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glEnd();
	//贴左边
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-30.0f, 0, -30.0f);
	glEnd();
	//贴前边
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0, -22.0f);
	glEnd();
	//贴右边
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0, -30.0f);
	glEnd();
	//绘制讲台
	//贴讲台纹理
	glBindTexture(GL_TEXTURE_2D, texsdesk);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f, -24.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glEnd();
	//画门
	glColor3f(0.521f, 0.121f, 0.0547f);
	glBindTexture(GL_TEXTURE_2D, texdoor);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //用于定义法线向量
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.9f, 0.0f, -25.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.9f, 14.0f, -25.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.9f, 14.0f, -19.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(39.9f, 0.0f, -19.0f);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}
//绘制桌子
void drawdesks()
{
	glBegin(GL_QUADS);

	//桌子前面
	glColor3f(0.82,0.68,0.5);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(2.0f, 5.0f, 2.0f);
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);

	//桌子后面
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);

	//桌子右边
	glVertex3f(2.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(2.0f, 5.0f, 2.0f);


	//桌子左边
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, -2.0f);

	//桌子上边
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);

	//桌子底部
	glVertex3f(2.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.0f, -2.0f);

	//右前桌腿
	//前
	glColor3f(0.6f, 0.4f, 0.0f);

	glVertex3f(1.8f, 5.0f, 1.6f);
	glVertex3f(1.4f, 5.0f, 1.6f);
	glVertex3f(1.4f, 0.0f, 1.6f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//后
	glVertex3f(1.8f, 5.0f, 1.2f);
	glVertex3f(1.4f, 5.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);

	//右
	glVertex3f(1.8f, 5.0f, 1.6f);
	glVertex3f(1.8f, 5.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//左
	glVertex3f(1.4f, 5.0f, 1.6f);
	glVertex3f(1.4f, 5.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.6f);

	//右后桌腿
	//前
	glVertex3f(1.8f, 5.0f, -1.2f);
	glVertex3f(1.4f, 5.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);

	//后
	glVertex3f(1.8f, 5.0f, -1.6f);
	glVertex3f(1.4f, 5.0f, -1.6f);
	glVertex3f(1.4f, 0.0f, -1.6f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//右
	glVertex3f(1.8f, 5.0f, -1.6f);
	glVertex3f(1.8f, 5.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//左
	glVertex3f(1.4f, 5.0f, -1.6f);
	glVertex3f(1.4f, 5.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.6f);

	//左前桌腿
	//前
	glVertex3f(-3.8f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 0.0f, 1.6f);
	glVertex3f(-3.8f, 0.0f, 1.6f);

	//后
	glVertex3f(-3.8f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.2f);

	//右
	glVertex3f(-3.8f, 5.0f, 1.6f);
	glVertex3f(-3.8f, 5.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.6f);

	//左
	glVertex3f(-3.4f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.6f);

	//左后桌腿

	//前
	glVertex3f(-3.8f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.2f);

	//后
	glVertex3f(-3.8f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 0.0f, -1.6f);
	glVertex3f(-3.8f, 0.0f, -1.6f);

	//右
	glVertex3f(-3.8f, 5.0f, -1.6f);
	glVertex3f(-3.8f, 5.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.6f);

	//左
	glVertex3f(-3.4f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.6f);



	glEnd();


}
//绘制椅子
void drawchairs()
{
	glColor3f(0.8, 0.68, 0.5);
	glBegin(GL_QUADS);

	//椅子前面
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(2.0f, 3.0f, 2.0f);
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);

	//椅子右边
	glVertex3f(2.0f, 3.0f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(2.0f, 3.0f, 2.0f);

	//椅子后边
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.0f, -2.0f);

	//椅子左边
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);

	//椅子上面
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);

	//椅子底部
	glVertex3f(2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(2.0f, 3.0f, -2.0f);

	//桌子前面的桌腿
	//桌腿前
	glVertex3f(1.8f, 3.0f, 1.6f);
	glVertex3f(1.4f, 3.0f, 1.6f);
	glVertex3f(1.4f, 0.0f, 1.6f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//后
	glVertex3f(1.8f, 3.0f, 1.2f);
	glVertex3f(1.4f, 3.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);

	//右
	glVertex3f(1.8f, 3.0f, 1.6f);
	glVertex3f(1.8f, 3.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//左
	glVertex3f(1.4f, 3.0f, 1.6f);
	glVertex3f(1.4f, 3.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.6f);

	//后面桌腿
	glVertex3f(1.8f, 3.0f, -1.2f);
	glVertex3f(1.4f, 3.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);

	//后
	glVertex3f(1.8f, 3.0f, -1.6f);
	glVertex3f(1.4f, 3.0f, -1.6f);
	glVertex3f(1.4f, 0.0f, -1.6f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//右
	glVertex3f(1.8f, 3.0f, -1.6f);
	glVertex3f(1.8f, 3.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//左
	glVertex3f(1.4f, 3.0f, -1.6f);
	glVertex3f(1.4f, 3.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.6f);

	//左前
	glVertex3f(-1.8f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 0.0f, 1.6f);
	glVertex3f(-1.8f, 0.0f, 1.6f);

	//后
	glVertex3f(-1.8f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.2f);

	//右
	glVertex3f(-1.8f, 3.0f, 1.6f);
	glVertex3f(-1.8f, 3.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.6f);

	//左
	glVertex3f(-1.4f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.6f);

	//左腿后 前

	//前
	glVertex3f(-1.8f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.2f);

	//后
	glVertex3f(-1.8f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 0.0f, -1.6f);
	glVertex3f(-1.8f, 0.0f, -1.6f);

	//右
	glVertex3f(-1.8f, 3.0f, -1.6f);
	glVertex3f(-1.8f, 3.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.6f);

	//左
	glVertex3f(-1.4f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.6f);

	glEnd();
}
//窗口刷新函数
void reshape(int we, int he)
{

	WinWidth = we;
	WinHeight = he;
	glViewport(0, 0, (GLsizei)we, (GLsizei)he);/*0 0指定了窗口的左下角位置
	//width,height表示视口矩形的宽度和高度，根据窗口的实时变化重绘窗口。*/
	glMatrixMode(GL_PROJECTION);//声明下一步操作为投影
	glLoadIdentity();//恢复初始坐标系
	gluPerspective(90.0f, (GLfloat)we / (GLfloat)he, 0.01f, 100.0f);
	/*透视投影：90.0f近裁剪平面与远裁剪平面的连线与视点的角度,
	也称视场角,0.01f近裁剪面到相机(视点)的距离,100.0f远裁剪面到相机(视点)的距离*/
	glMatrixMode(GL_MODELVIEW);//声明下一步为对模型视图的操作
	glLoadIdentity();
	/*第一组eyex, eyey,eyez 相机在世界坐标的位置(视点)
	第二组centerx,centery,centerz 相机镜头对准的物体在世界坐标的位置
	第三组upx,upy,upz 相机向上的方向在世界坐标中的方向
	把相机想象成为你自己的脑袋：
	第一组数据就是脑袋的位置
	第二组数据就是眼睛看的物体的位置
	第三组就是头顶朝向的方向（因为你可以歪着头看同一个物体）*/
	gluLookAt(myEye.x, myEye.y, myEye.z, vPoint.x + 30 * sin(vAngle), vPoint.y, -30 * cos(vAngle), up.x, up.y, up.z);//0.0f, 1.0f, 0.0f);
}
//显示函数
void myDisplay()
{
	// 清除屏幕
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//调用绘制函数
	drawscence();
	for (int i = -3; i <= 3; i += 2) {
		for (int j = -3; j <= 3; j += 2) {
			glPushMatrix();
			glTranslatef(i * 5.0f, 3.0f, j * 3.0 + 2.2f);
			glScalef(0.8f, 0.8f, 0.5f);
			drawdesks();
			glPopMatrix();
		}
	}
	for (int i = -3; i <= 3; i += 2) {
		for (int j = -3; j <= 3; j += 2) {
			glPushMatrix();
			glTranslatef(i * 5.0f + 0.2f, 3.0f, j * 3.0 + 5.5f);
			glScalef(0.8f, 0.8f, 0.5f);
			drawchairs();
			glPopMatrix();
		}
	}
	glFlush();//清空缓冲区
}
//响应普通键盘操作，w，s，a，d以及退出esc键
GLvoid OnKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 97://a 向左
		myEye.x -= 0.5;
		vPoint.x -= 0.5;
		if (myEye.x <= -38)
			myEye.x = -38;
		break;
	case 100://d 向右
		myEye.x += 0.5;
		vPoint.x += 0.5;
		if (myEye.x >= 38)
			myEye.x = 38;
		break;
	case 119://w 向前
		myEye.z -= 0.5;
		if (myEye.z <= -28)
			myEye.z = -28;
		break;
	case 115://s 向后
		myEye.z += 0.5;
		if (myEye.z >= 28)
			myEye.z = 28;
		break;
	case 27://esc
		exit(0);

	}
	reshape(WinWidth, WinHeight);//窗口刷新
	glutPostRedisplay();

}
//响应特殊键盘操作
GLvoid OnUPDOWN(int key, int x, int y)//上下左右
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		vAngle -= 0.05;
		break;
	case GLUT_KEY_RIGHT:
		vAngle += 0.05;
		break;
	case GLUT_KEY_UP:
		myEye.y += 0.5;
		if (myEye.y >= 30)
			myEye.y = 30;

		break;
	case GLUT_KEY_DOWN:
		myEye.y -= 0.5;
		if (myEye.y <= 0)
			myEye.y = 30;
		break;
	}
	reshape(WinWidth, WinHeight);
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	myEye.x = 0;
	myEye.y = 15;
	myEye.z = 25;
	vPoint.x = 0;
	vPoint.y = 15;
	vPoint.z = -30;
	up.x = 0;
	up.y = 1;
	up.z = 0;
	vAngle = 0;
	glEnable(GL_DEPTH_TEST);//启用了之后，OpenGL在绘制的时候就会检查，当前像素前面是否有别的像素，如果别的像素挡道了它，那它就不会绘制，也就是说，OpenGL就只绘制最前面的一层。
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);//定义显示方式，GLUT_RGBA指定一个RGBA窗口，GLUT_SINGLE.单缓冲区窗口
	glutInitWindowPosition(0, 0);//设置初始窗口的位置
	glutInitWindowSize(800, 600);//设置窗口大小
	glutCreateWindow("classroom");
	glutDisplayFunc(&myDisplay);//注册显示回调函数,包含重绘场景所需的所有代码
	glutReshapeFunc(reshape);//注册窗口改变回调函数
	glutKeyboardFunc(OnKeyboard);//注册键盘响应事件
	glutSpecialFunc(OnUPDOWN);//对键盘上特殊的4个方向按键的响应函数

	texblackboard = load_texture("blackboard.bmp");
	texwindow = load_texture("window.bmp");
	texsound = load_texture("sound.bmp");
	texceiling = load_texture("ceiling.bmp");
	texdoor = load_texture("door.bmp");
	texfloor = load_texture("floor.bmp");
	texbackwall = load_texture("backwall.bmp");
	texpole = load_texture("pole.bmp");
	texairfro = load_texture("airconditionfront.bmp");
	texairback = load_texture("airconditionback.bmp");
	texhighland = load_texture("gaodi.bmp");
	texsdesk = load_texture("sdesk.bmp");
	texclock = load_texture("clock.bmp");
	texcuc = load_texture("CUC.bmp");
	texcsc = load_texture("CSC.bmp");
	texlmy = load_texture("lmy.bmp");

	glutMainLoop();//进入事件处理循环
	return 0;
}