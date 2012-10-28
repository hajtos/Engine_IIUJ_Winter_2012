/** @file utils_shaders.cpp
 *  @brief impleentation of utilities for shader loading and management, simple C style
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#include "stdafx.h"
#include "utils.h"
#include "utils_shaders.h"

//////////////////////////////////////////////////////////////////////////
// globals
#define UTILS_MAX_USED_SHADERS 100

static GLuint utilsUsedShaders[UTILS_MAX_USED_SHADERS*2];
static GLuint utilsUsedPrograms[UTILS_MAX_USED_SHADERS];
static GLuint utilsNextShaderToUse = 0;
static GLuint utilsNextProgramToUse = 0;

//////////////////////////////////////////////////////////////////////////
// from Lighthouse3D tutorial
char *ReadAllTextFromFile(const char *fname) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fname == NULL) return NULL;

	fopen_s(&fp, fname,"rt");

	if (fp == NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	count = ftell(fp);
	rewind(fp);

	if (count > 0) {
		content = (char *)malloc(sizeof(char) * (count+1));
		count = (int)fread(content,sizeof(char), count, fp);
		content[count] = '\0';
	}
	fclose(fp);

	return content;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

//////////////////////////////////////////////////////////////////////////
// based on code frm lighthouse3D
bool utLoadAndBuildShader(const char *vsFile, const char *fsFile, GLuint *progID)
{
	char *vs,*fs;
	GLuint v, f, p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);	

	vs = ReadAllTextFromFile(vsFile);
	if (vs == NULL)
	{
		utLOG_ERROR("cannot load Vertex Shader from file: %s\n", vsFile);
		return false;
	}
	fs = ReadAllTextFromFile(fsFile);
	if (fs == NULL)
	{
		utLOG_ERROR("cannot load Fragment Shader from file: %s\n", fsFile);
		free(vs);
		return false;
	}

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);    
	glShaderSource(f, 1, &ff,NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
    printShaderInfoLog(v);
	glCompileShader(f);
    printShaderInfoLog(f);

	p = glCreateProgram();

	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	int success = 0;
	glGetProgramiv(p, GL_LINK_STATUS, &success);
	if (!success) {
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;

		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(p, infologLength, &charsWritten, infoLog);
			utLOG_ERROR("error in building program! \n%s",infoLog);
			free(infoLog);
			return false;
		}
	}
	
	utLOG("GLSL program (id %d) loaded from %s and %s!\n", p, vsFile, fsFile);
	
	//glUseProgram(p);

	utilsUsedShaders[utilsNextShaderToUse++] = v;
	utilsUsedShaders[utilsNextShaderToUse++] = f;
	utilsUsedPrograms[utilsNextProgramToUse++] = p;
	*progID = p;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// cleanup
void utDeleteAllUsedShaders()
{
	for (unsigned int i = 0; i < utilsNextShaderToUse; ++i)
	{
		glDeleteShader(utilsUsedShaders[i]);
		utilsUsedShaders[i] = 0;
	}
	for (unsigned int i = 0; i < utilsNextProgramToUse; ++i)
	{
		glDeleteProgram(utilsUsedPrograms[i]);
		utilsUsedPrograms[i] = 0;
	}

	// reset
	utilsNextShaderToUse = 0;
	utilsNextProgramToUse = 0;

	utLOG("all used shaders deleted!");
}