

#include <stdio.h>
#ifdef NANOVG_GLEW
#	include <GL/glew.h>
#endif
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "nanovg_gl_utils.h"

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

struct ExampleData
{
    int image;
    int font;
} ;

void LoadData(struct ExampleData* data,NVGcontext* vg)
{
    data->image = nvgCreateImage(vg, "../example/images/image1.jpg", 0);
    data->font = nvgCreateFont(vg, "sans-bold", "../example/Roboto-Bold.ttf");

}
void FreeData(struct ExampleData* data,NVGcontext* vg)
{
    nvgDeleteImage(vg, data->image);
}

void RenderExample(struct ExampleData* data, NVGcontext* vg,int winWidth,int winHeight,float ratio)
{
    int imgWidth,imgHeight;
    nvgImageSize(vg, data->image,&imgWidth,&imgHeight);
    
    nvgBeginFrame(vg, winWidth, winHeight, ratio);
    {//clip rounded rect
        int drawx = 30;
        int drawy = 30;
        
        
        //Hollow out clipping area
        nvgGlobalCompositeOperation(vg,NVG_DESTINATION_OUT);
        nvgFillColor(vg,nvgRGBf(1,1,1));
        nvgBeginPath(vg);
        nvgRoundedRect(vg,drawx,drawy,imgWidth,imgHeight,30);
        nvgFill(vg);
        
        
        //draw image
        nvgGlobalCompositeOperation(vg,NVG_DESTINATION_ATOP);
        NVGpaint paint = nvgImagePattern(vg, drawx ,drawy, imgWidth, imgHeight, 0, data->image , 1);
        nvgFillPaint(vg,paint);
        nvgBeginPath(vg);
        nvgRect(vg,drawx,drawy,imgWidth,imgHeight);
        nvgFill(vg);
        
    
    }
    
    {//clip text area
        int drawx = 230;
        int drawy = 30;
        
        
        //Hollow out clipping area
        nvgGlobalCompositeOperation(vg,NVG_DESTINATION_OUT);
        nvgFillColor(vg,nvgRGBf(1,1,1));
        nvgFontFaceId(vg,data->font);
        nvgFontSize(vg,40);
        nvgTextAlign(vg,NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
        nvgTextBox(vg, drawx, drawy , imgWidth , "Textured Text" , 0);
        
        //draw image
        nvgGlobalCompositeOperation(vg,NVG_DESTINATION_ATOP);
        NVGpaint paint = nvgImagePattern(vg, drawx ,drawy, imgWidth, imgHeight, 0, data->image , 1);
        nvgFillPaint(vg,paint);
        nvgBeginPath(vg);
        nvgRect(vg,drawx,drawy,imgWidth,imgHeight);
        nvgFill(vg);
        
        
    }
    nvgEndFrame(vg);
}



int main()
{
	GLFWwindow* window;
	NVGcontext* vg = NULL;
	double prevt = 0, cpuTime = 0;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
    struct ExampleData exampleData;


	glfwSetErrorCallback(errorcb);
#ifndef _WIN32 // don't require this on win32, and works with more cards
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	window = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
//	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
#endif
    
    vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
    
    LoadData(&exampleData,vg);

	glfwSwapInterval(0);

    
	glfwSetTime(0);
	prevt = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
        
        glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

        RenderExample(&exampleData,vg,winWidth,winHeight,pxRatio);

		// Measure the CPU time taken excluding swap buffers (as the swap may wait for GPU)
		cpuTime = glfwGetTime() - t;


		glfwSwapBuffers(window);
		glfwPollEvents();
    }
    FreeData(&exampleData,vg);

	nvgDeleteGL3(vg);


	glfwTerminate();
	return 0;
}
