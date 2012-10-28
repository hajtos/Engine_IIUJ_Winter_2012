/** @file utils_shaders.h
 *  @brief utilities for shader loading and management, simple C style
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once

/** loads whole program composed of vertex shader and fragment shader
	@param vsFile file name for vertex shader
	@param fsFile file name for fragment shader
	@param progID id of new program
	@return false on error and errors are written by LOG* macros.
*/
bool utLoadAndBuildShader(const char *vsFile, const char *fsFile, GLuint *progID);
/** internally utLoadAndBuildShaderProgram uses array of shaders ids, so at the end of application
    one should delete them by calling that function */
void utDeleteAllUsedShaders();


/** simple function for sending uniform INT vars - not optimal */
inline void utSetUniform1i(GLuint prog, const char *var, int value)
{
	glUniform1i(glGetUniformLocation(prog, var), value);
}

/** simple function for sending uniform FLOAT vars - not optimal */
inline void utSetUniform1f(GLuint prog, const char *var, float value)
{
	glUniform1f(glGetUniformLocation(prog, var), value);
}