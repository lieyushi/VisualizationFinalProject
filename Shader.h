#ifndef _SHADER_H
#define _SHADER_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
class Shader
{

public:

	Shader();

	GLuint shaderCompileFromFile(GLenum type, const char *filePath);
	void shaderAttachFromFile(GLuint program, GLenum type, const char *filePath);
	//void sceneInit(GLint g_program);
	char* shaderLoadSource(const char *filePath);

/*private:
	GLuint vertexShader;
	GLuint fragmentShader;*/

};

#endif