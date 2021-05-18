#include<gl/glut.h>
#include<windows.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
GLint	WinWidth;
GLint	WinHeight;

//�����ӵ�ṹ
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
//����λͼ��Ϊ�������غ���
#define BMP_Header_Length 54

//�ж�һ�����Ƿ���2�������η�
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

	// ���ļ������ʧ�ܣ�����    
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�    
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���    
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�    
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������    
	fread(pixels, total_bytes, 1, pFile);

	// �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ���2�������η�������Ҫ��������    
	// ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����    
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (power_of_two(width)
			|| power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������    
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���    
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�    
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������    
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height    
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������    
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�    
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������    
	free(pixels);
	return texture_ID;
}
//�������������������
GLuint texblackboard, texwindow, texdesk, texsound;
GLuint texceiling, texdoor, texfloor, texbackwall, texpole;
GLuint texairfro, texairback, texhighland, texsdesk, texclock, texcsc,texcuc,texlmy;

//���ƽ�������󳡾�
void drawscence()
{


	/*glTexCoord2f����ͼ��ʱָ����������꣬��һ����X�����꣬0.0���������࣬0.5��������е㣬1.0��������Ҳ࣬
	�ڶ�����Y�����꣬0.0������ĵײ���0.5��������е㣬1.0������Ķ�����
	Ϊ�˽�������ȷ��ӳ�䵽�ı����ϣ������뽫��������Ͻ�ӳ�䵽�ı��ε����Ͻǣ���������Ͻ�ӳ�䵽�ı��ε����Ͻǣ�
	��������½�ӳ�䵽�ı��ε����½ǣ���������½�ӳ�䵽�ı��ε����½ǣ����������������X��0.0��Y��1.0f���ı��ε����϶�����X��-1.0��Y��1.0��*/
	
	
	glEnable(GL_TEXTURE_2D);//����2D������
	//�컨��
	glBindTexture(GL_TEXTURE_2D, texceiling);//������
	glBegin(GL_QUADS);//���ı���
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 30.0f, 30.0f);//����
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);//����
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);//����
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 30.0f, 30.0f);//����
	glEnd();


	//���Ƶذ�
	glBindTexture(GL_TEXTURE_2D, texfloor);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();

	//�������ǽ
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glEnd();

	//������ߴ���
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

	//�����ұ�ǽ
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glEnd();

	//�����ұߴ���
	glBindTexture(GL_TEXTURE_2D, texwindow);
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0f, 0.0f);	                  //���ڶ��巨������
	glTexCoord2f(1.0f, 0.0f); glVertex3f(39.5, 10, 10);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.5, 20, 10);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.5, 20, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.5, 10, 0);
	glEnd();

	//���ƺ��ǽ
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, 30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, 30.0f);
	glEnd();

	//CUC
	glBindTexture(GL_TEXTURE_2D, texcuc);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.8f, 15.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-4.8f, 20.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(4.8f, 20.0f, 30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(4.8f, 15.0f, 30.0f);
	glEnd();

	//CSC
	glBindTexture(GL_TEXTURE_2D, texcsc);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-15.0f, 10.0f, 30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-15.0f, 15.0f, 30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(15.0f, 15.0f, 30.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(15.0f, 10.0f, 30.0f);
	glEnd();

	//����ǰ��ǽ
	glBindTexture(GL_TEXTURE_2D, texbackwall);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0f, 30.0f, -30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(40.0f, 30.0f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
	glEnd();

	//����
	glBindTexture(GL_TEXTURE_2D, texclock);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(23.0f, 18.0f, -29.8f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(23.0f, 20.0f, -29.8f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(25.0f, 20.0f, -29.8f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(25.0f, 18.0f, -29.8f);
	glEnd();

	//���ƿյ�
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

	//���ƽ�������ʯ��ǰ������
	glBindTexture(GL_TEXTURE_2D, texpole);
	for (int i = 0; i <= 1; i++)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		//glColor3f(255, 255, 255);
		//ʯ���ϱ���
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-40.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0f + i * 78, 30.0f, -4.0f);
		glEnd();
		//ʯ��ǰ����
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 0.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 78, 0.0f, -4.0f);
		glEnd();
		//ʯ�������
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + i * 78, 0.0f, -6.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 78, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 78, 0.0f, -6.0f);
		glEnd();
		//ʯ���ұ���
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0 + i * 76, 0.0f, -4.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0 + i * 76, 30.0f, -4.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + i * 76, 30.0f, -6.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + i * 76, 0.0f, -6.0f);
		glEnd();
	}
	//���ƽ�������ʯ�����������
	for (int j = 0; j <= 1; j++)
	{

		glColor3f(1.0f, 1.0f, 1.0f);
		//glColor3f(255, 255, 255);
		//ʯ���ϱ���
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0f + j * 78, 30.0f, 14.0f);
		glEnd();
		//ʯ��ǰ����
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 0.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 78, 0.0f, 14.0f);
		glEnd();
		//ʯ�������
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-40.0 + j * 78, 0.0f, 12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-40.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 78, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 78, 0.0f, 12.0f);
		glEnd();
		//�ұ���
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f); //���ڶ��巨������
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-38.0 + j * 76, 0.0f, 14.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-38.0 + j * 76, 30.0f, 14.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-38.0 + j * 76, 30.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-38.0 + j * 76, 0.0f, 12.0f);
		glEnd();
	}

	//���ƺڰ�
	glBindTexture(GL_TEXTURE_2D, texblackboard);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0, 8.0f, -29.9f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0, 18.0f, -29.9f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0, 8.0f, -29.9f);
	glEnd();

	//lmy
	glBindTexture(GL_TEXTURE_2D, texlmy);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-5.0, 20.0f, -29.9f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-5.0, 25.0f, -29.9f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(5.0, 25.0f, -29.9f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(5.0, 20.0f, -29.9f);
	glEnd();
	
	//���ƽ���ǰ��һ��ߵز�������
	glBindTexture(GL_TEXTURE_2D, texhighland);
	//������
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glEnd();
	//�����
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-30.0f, 0, -30.0f);
	glEnd();
	//��ǰ��
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0, -22.0f);
	glEnd();
	//���ұ�
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(30.0f, 0, -22.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(30.0f, 1.5f, -22.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(30.0f, 1.5f, -30.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(30.0f, 0, -30.0f);
	glEnd();
	//���ƽ�̨
	//����̨����
	glBindTexture(GL_TEXTURE_2D, texsdesk);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f, -24.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -28.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-7.5f, 1.5f, -28.0f);
	glEnd();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.5f, 9.5f, -24.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(7.5f, 9.5f, -26.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(7.5f, 9.5f, -24.0f);
	glEnd();
	//����
	glColor3f(0.521f, 0.121f, 0.0547f);
	glBindTexture(GL_TEXTURE_2D, texdoor);
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f); //���ڶ��巨������
	glTexCoord2f(0.0f, 0.0f); glVertex3f(39.9f, 0.0f, -25.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(39.9f, 14.0f, -25.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(39.9f, 14.0f, -19.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(39.9f, 0.0f, -19.0f);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}
//��������
void drawdesks()
{
	glBegin(GL_QUADS);

	//����ǰ��
	glColor3f(0.82,0.68,0.5);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(2.0f, 5.0f, 2.0f);
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);

	//���Ӻ���
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);

	//�����ұ�
	glVertex3f(2.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(2.0f, 5.0f, 2.0f);


	//�������
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, -2.0f);

	//�����ϱ�
	glVertex3f(2.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, 2.0f);
	glVertex3f(-4.0f, 5.4f, -2.0f);
	glVertex3f(2.0f, 5.4f, -2.0f);

	//���ӵײ�
	glVertex3f(2.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.0f, 2.0f);
	glVertex3f(-4.0f, 5.0f, -2.0f);
	glVertex3f(2.0f, 5.0f, -2.0f);

	//��ǰ����
	//ǰ
	glColor3f(0.6f, 0.4f, 0.0f);

	glVertex3f(1.8f, 5.0f, 1.6f);
	glVertex3f(1.4f, 5.0f, 1.6f);
	glVertex3f(1.4f, 0.0f, 1.6f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(1.8f, 5.0f, 1.2f);
	glVertex3f(1.4f, 5.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);

	//��
	glVertex3f(1.8f, 5.0f, 1.6f);
	glVertex3f(1.8f, 5.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(1.4f, 5.0f, 1.6f);
	glVertex3f(1.4f, 5.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.6f);

	//�Һ�����
	//ǰ
	glVertex3f(1.8f, 5.0f, -1.2f);
	glVertex3f(1.4f, 5.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);

	//��
	glVertex3f(1.8f, 5.0f, -1.6f);
	glVertex3f(1.4f, 5.0f, -1.6f);
	glVertex3f(1.4f, 0.0f, -1.6f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(1.8f, 5.0f, -1.6f);
	glVertex3f(1.8f, 5.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(1.4f, 5.0f, -1.6f);
	glVertex3f(1.4f, 5.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.6f);

	//��ǰ����
	//ǰ
	glVertex3f(-3.8f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 0.0f, 1.6f);
	glVertex3f(-3.8f, 0.0f, 1.6f);

	//��
	glVertex3f(-3.8f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.2f);

	//��
	glVertex3f(-3.8f, 5.0f, 1.6f);
	glVertex3f(-3.8f, 5.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.2f);
	glVertex3f(-3.8f, 0.0f, 1.6f);

	//��
	glVertex3f(-3.4f, 5.0f, 1.6f);
	glVertex3f(-3.4f, 5.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.2f);
	glVertex3f(-3.4f, 0.0f, 1.6f);

	//�������

	//ǰ
	glVertex3f(-3.8f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.2f);

	//��
	glVertex3f(-3.8f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 0.0f, -1.6f);
	glVertex3f(-3.8f, 0.0f, -1.6f);

	//��
	glVertex3f(-3.8f, 5.0f, -1.6f);
	glVertex3f(-3.8f, 5.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.2f);
	glVertex3f(-3.8f, 0.0f, -1.6f);

	//��
	glVertex3f(-3.4f, 5.0f, -1.6f);
	glVertex3f(-3.4f, 5.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.2f);
	glVertex3f(-3.4f, 0.0f, -1.6f);



	glEnd();


}
//��������
void drawchairs()
{
	glColor3f(0.8, 0.68, 0.5);
	glBegin(GL_QUADS);

	//����ǰ��
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(2.0f, 3.0f, 2.0f);
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);

	//�����ұ�
	glVertex3f(2.0f, 3.0f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(2.0f, 3.0f, 2.0f);

	//���Ӻ��
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.0f, -2.0f);

	//�������
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);

	//��������
	glVertex3f(2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, 2.0f);
	glVertex3f(-2.0f, 3.4f, -2.0f);
	glVertex3f(2.0f, 3.4f, -2.0f);

	//���ӵײ�
	glVertex3f(2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.0f, 2.0f);
	glVertex3f(-2.0f, 3.0f, -2.0f);
	glVertex3f(2.0f, 3.0f, -2.0f);

	//����ǰ�������
	//����ǰ
	glVertex3f(1.8f, 3.0f, 1.6f);
	glVertex3f(1.4f, 3.0f, 1.6f);
	glVertex3f(1.4f, 0.0f, 1.6f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(1.8f, 3.0f, 1.2f);
	glVertex3f(1.4f, 3.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);

	//��
	glVertex3f(1.8f, 3.0f, 1.6f);
	glVertex3f(1.8f, 3.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.2f);
	glVertex3f(1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(1.4f, 3.0f, 1.6f);
	glVertex3f(1.4f, 3.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.2f);
	glVertex3f(1.4f, 0.0f, 1.6f);

	//��������
	glVertex3f(1.8f, 3.0f, -1.2f);
	glVertex3f(1.4f, 3.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);

	//��
	glVertex3f(1.8f, 3.0f, -1.6f);
	glVertex3f(1.4f, 3.0f, -1.6f);
	glVertex3f(1.4f, 0.0f, -1.6f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(1.8f, 3.0f, -1.6f);
	glVertex3f(1.8f, 3.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.2f);
	glVertex3f(1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(1.4f, 3.0f, -1.6f);
	glVertex3f(1.4f, 3.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.2f);
	glVertex3f(1.4f, 0.0f, -1.6f);

	//��ǰ
	glVertex3f(-1.8f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 0.0f, 1.6f);
	glVertex3f(-1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(-1.8f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.2f);

	//��
	glVertex3f(-1.8f, 3.0f, 1.6f);
	glVertex3f(-1.8f, 3.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.2f);
	glVertex3f(-1.8f, 0.0f, 1.6f);

	//��
	glVertex3f(-1.4f, 3.0f, 1.6f);
	glVertex3f(-1.4f, 3.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.2f);
	glVertex3f(-1.4f, 0.0f, 1.6f);

	//���Ⱥ� ǰ

	//ǰ
	glVertex3f(-1.8f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.2f);

	//��
	glVertex3f(-1.8f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 0.0f, -1.6f);
	glVertex3f(-1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(-1.8f, 3.0f, -1.6f);
	glVertex3f(-1.8f, 3.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.2f);
	glVertex3f(-1.8f, 0.0f, -1.6f);

	//��
	glVertex3f(-1.4f, 3.0f, -1.6f);
	glVertex3f(-1.4f, 3.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.2f);
	glVertex3f(-1.4f, 0.0f, -1.6f);

	glEnd();
}
//����ˢ�º���
void reshape(int we, int he)
{

	WinWidth = we;
	WinHeight = he;
	glViewport(0, 0, (GLsizei)we, (GLsizei)he);/*0 0ָ���˴��ڵ����½�λ��
	//width,height��ʾ�ӿھ��εĿ�Ⱥ͸߶ȣ����ݴ��ڵ�ʵʱ�仯�ػ洰�ڡ�*/
	glMatrixMode(GL_PROJECTION);//������һ������ΪͶӰ
	glLoadIdentity();//�ָ���ʼ����ϵ
	gluPerspective(90.0f, (GLfloat)we / (GLfloat)he, 0.01f, 100.0f);
	/*͸��ͶӰ��90.0f���ü�ƽ����Զ�ü�ƽ����������ӵ�ĽǶ�,
	Ҳ���ӳ���,0.01f���ü��浽���(�ӵ�)�ľ���,100.0fԶ�ü��浽���(�ӵ�)�ľ���*/
	glMatrixMode(GL_MODELVIEW);//������һ��Ϊ��ģ����ͼ�Ĳ���
	glLoadIdentity();
	/*��һ��eyex, eyey,eyez ��������������λ��(�ӵ�)
	�ڶ���centerx,centery,centerz �����ͷ��׼�����������������λ��
	������upx,upy,upz ������ϵķ��������������еķ���
	����������Ϊ���Լ����Դ���
	��һ�����ݾ����Դ���λ��
	�ڶ������ݾ����۾����������λ��
	���������ͷ������ķ�����Ϊ���������ͷ��ͬһ�����壩*/
	gluLookAt(myEye.x, myEye.y, myEye.z, vPoint.x + 30 * sin(vAngle), vPoint.y, -30 * cos(vAngle), up.x, up.y, up.z);//0.0f, 1.0f, 0.0f);
}
//��ʾ����
void myDisplay()
{
	// �����Ļ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//���û��ƺ���
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
	glFlush();//��ջ�����
}
//��Ӧ��ͨ���̲�����w��s��a��d�Լ��˳�esc��
GLvoid OnKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 97://a ����
		myEye.x -= 0.5;
		vPoint.x -= 0.5;
		if (myEye.x <= -38)
			myEye.x = -38;
		break;
	case 100://d ����
		myEye.x += 0.5;
		vPoint.x += 0.5;
		if (myEye.x >= 38)
			myEye.x = 38;
		break;
	case 119://w ��ǰ
		myEye.z -= 0.5;
		if (myEye.z <= -28)
			myEye.z = -28;
		break;
	case 115://s ���
		myEye.z += 0.5;
		if (myEye.z >= 28)
			myEye.z = 28;
		break;
	case 27://esc
		exit(0);

	}
	reshape(WinWidth, WinHeight);//����ˢ��
	glutPostRedisplay();

}
//��Ӧ������̲���
GLvoid OnUPDOWN(int key, int x, int y)//��������
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
	glEnable(GL_DEPTH_TEST);//������֮��OpenGL�ڻ��Ƶ�ʱ��ͻ��飬��ǰ����ǰ���Ƿ��б�����أ����������ص��������������Ͳ�����ƣ�Ҳ����˵��OpenGL��ֻ������ǰ���һ�㡣
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);//������ʾ��ʽ��GLUT_RGBAָ��һ��RGBA���ڣ�GLUT_SINGLE.������������
	glutInitWindowPosition(0, 0);//���ó�ʼ���ڵ�λ��
	glutInitWindowSize(800, 600);//���ô��ڴ�С
	glutCreateWindow("classroom");
	glutDisplayFunc(&myDisplay);//ע����ʾ�ص�����,�����ػ泡����������д���
	glutReshapeFunc(reshape);//ע�ᴰ�ڸı�ص�����
	glutKeyboardFunc(OnKeyboard);//ע�������Ӧ�¼�
	glutSpecialFunc(OnUPDOWN);//�Լ����������4�����򰴼�����Ӧ����

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

	glutMainLoop();//�����¼�����ѭ��
	return 0;
}