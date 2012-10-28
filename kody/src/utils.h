/** @file utils.h
 *  @brief useful utilities for opengl based apps
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#pragma once

/** logging will be by using simple "printf" function */
#define UT_LOG_WITH_PRINTF

#ifdef UT_LOG_WITH_PRINTF
	#define utLOG(msg, ...) { printf("%s: ", __FUNCTION__); printf(msg, __VA_ARGS__); printf("\n"); }
	#define utLOG_ERROR(msg, ...) { printf("ERR in %s, %d: ", __FUNCTION__, __LINE__); printf(msg, __VA_ARGS__); printf("\n"); }
#else
// TODO: implement
#endif

///////////////////////////////////////////////////////////////////////////////
bool utInitOpenGL20();

///////////////////////////////////////////////////////////////////////////////
// 2D and TEXT MODE
void utSetOrthographicProjection(int scrW, int scrH);
void utResetPerspectiveProjection();
void utDrawText2D(float x, float y, void *font, char *string);

// simplier version for default font...
inline void utDrawText2D(float x, float y, char *string) {
	utDrawText2D(x, y, (void *)GLUT_BITMAP_8_BY_13, string);
}

void utCalculateAndPrintFps(float x, float y);


		