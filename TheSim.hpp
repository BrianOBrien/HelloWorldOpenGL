#ifndef The_Sim
#define The_Sim

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <GLFW/glfw3.h>

#define M_PI ((float) 3.1415926535F)
#define TWOPI ((float)M_PI * 2.0f)
#define PIBYTWO ((float)M_PI / 2.0f)
#define SPHERE_DIV (16)

#define LIGHTING


typedef struct {
    int width;
    int height;
    GLubyte* image;
} texture;

class TheSim
{
private:

    GLfloat day = 0.0F;
    GLfloat month = 0.0F;
    GLfloat year = 0.0F;

    texture* sun;
    texture* moon;
    texture* earth;
    /*
    int getResource(void) {

        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDB_PNG1), L"PNG");
        if (!hResInfo) {
            std::cerr << "Failed to find resource." << std::endl;
            return 1;
        }
        HGLOBAL hResData = LoadResource(NULL, hResInfo);
        if (!hResData) {
            std::cerr << "Failed to load resource." << std::endl;
            return 1;
        }

        LPVOID pResourceData = LockResource(hResData);
        if (!pResourceData) {
            std::cerr << "Failed to lock resource." << std::endl;
            return 1;
        }
        DWORD dwSize = SizeofResource(NULL, hResInfo);
        if (dwSize == 0) {
            std::cerr << "Resource size is zero." << std::endl;
            return 1;
        }

        std::ofstream outfile("image_data.bin", std::ios::binary);
        if (!outfile) {
            std::cerr << "Failed to open output file." << std::endl;
            return 1;
        }
        outfile.write(static_cast<char*>(pResourceData), dwSize);
        outfile.close();
        std::cout << "Resource data written" << std::endl;
        return 0;
    }
    */
    texture* readPPMImage(const char* name)
    {
        FILE* fp;
        char buffer[256];
        int i, width, height, s;
        texture* text = NULL;
        GLubyte* p;
        errno_t err;

        err = fopen_s(&fp, (const char*)name, (const char*)"rb");
        if (err == 0) {

            fgets(buffer, 255, fp);

            for (;;) {
                fgets(buffer, 255, fp);
                if (*buffer != '#') break;
            }
            sscanf_s(buffer, "%lu %lu", &width, &height);
            fgets(buffer, 255, fp);

            s = width * height * 3;
            text = (texture*)malloc(sizeof(texture));
            if (text != NULL)
            {
                text->width = width;
                text->height = height;
                p = text->image = (GLubyte*)malloc(s);
                if (p != NULL)
                {
                    for (i = 0; i < s; ++i) {
                        *p = (GLubyte)getc(fp);
                        ++p;
                    }
                }
                fclose(fp);
            }
        }
        return(text);
    }
    void func(int width, int x, int height, int y, int segments)
    {
        GLfloat fx, fy;

        fx = (((float)width * (float)x / (float)segments) / (float)width);
        fy = (((float)height * (float)y / (float)segments) / (float)height);

        glTexCoord2f(fx, fy);
    }
    void makeSphere(int name, GLfloat* s, texture* text)
    {
        GLfloat x, y, z;
        GLfloat r, itheta, jtheta, iinc, jinc;
        int i, j, imageWidth, imageHeight;

        imageWidth = text->width;
        imageHeight = text->height;

        iinc = M_PI / (float)SPHERE_DIV;
        jinc = TWOPI / (float)SPHERE_DIV;

        glNewList(name, GL_COMPILE);

        glPushMatrix();

        glScalef(s[0], s[1], s[2]);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            3,
            text->width,
            text->height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            text->image);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);

        for (jtheta = 0.0f, j = 0; j < SPHERE_DIV; ++j, jtheta += jinc) {

            glBegin(GL_TRIANGLE_STRIP);

            func(imageWidth, j, imageHeight, 0, SPHERE_DIV);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 1.0f);

            for (itheta = PIBYTWO - iinc, i = 1; i < SPHERE_DIV; ++i, itheta -= iinc) {

                z = (float)sin((float)itheta);
                r = (float)cos((float)itheta);

                x = r * (float)cos((float)jtheta);
                y = r * (float)sin((float)jtheta);

                func(imageWidth, j, imageHeight, i, SPHERE_DIV);
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);

                x = r * (float)cos((float)(jtheta + jinc));
                y = r * (float)sin((float)(jtheta + jinc));

                func(imageWidth, j + 1, imageHeight, i, SPHERE_DIV);
                glNormal3f(x, y, z);
                glVertex3f(x, y, z);

            }

            func(imageWidth, j, imageHeight, SPHERE_DIV, SPHERE_DIV);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(0.0f, 0.0f, -1.0f);

            glEnd();
        }

        glPopMatrix();

        glEndList();
    }
    void makeMoon(void)
    {
        GLfloat sca[3] = { .15f,.15f,.15f };
        const char* fname = "D:\\Development\\HelloWorldOpenGL\\Resources\\moon.ppm";
        moon = readPPMImage(fname);
        makeSphere(3, sca, moon);
    }
    void makeEarth(void)
    {
        GLfloat sca[3] = { .4f,.4f,.4f };
        const char* fname = "D:\\Development\\HelloWorldOpenGL\\Resources\\earth.ppm";
        earth = readPPMImage(fname);
        makeSphere(2, sca, earth);
    }
    void makeSun(void)
    {
        GLfloat sca[3] = { 1.0f,1.0f,1.0f };
        const char* fname = "D:\\Development\\HelloWorldOpenGL\\Resources\\sun.ppm";
        sun = readPPMImage(fname);
        makeSphere(1, sca, sun);
    }
    GLvoid createObjects()
    {
        makeSun();
        makeEarth();
        makeMoon();
    }
    /*
    GLvoid myShader(void)
    {
        // Render pass from light's perspective
        glUseProgram(depthShaderProgram);
        glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Set light's view and projection matrices
        glm::mat4 lightView = glm::lookAt(lightPosition, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, near, far);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Render scene geometry
        for (auto& object : sceneObjects) {
            glm::mat4 model = object.getModelMatrix();
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));
            object.render();
        }

        // Render pass from camera's perspective
        glUseProgram(shaderProgram);
        glViewport(0, 0, screenWidth, screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view and projection matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Set light space matrix and shadow map texture
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(shadowMapLocation, 1);

        // Render scene geometry with shadow mapping
        for (auto& object : sceneObjects) {
            glm::mat4 model = object.getModelMatrix();
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));
            object.render();
        }

    }
    */
    GLvoid initializeGL(GLsizei width, GLsizei height)
    {
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
#ifdef LIGHTING
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
#endif
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_TEXTURE_2D);

        createObjects();
    }
    void DrawSun(void)
    {
        glCallList(1);
    }
    void DrawEarth(void)
    {
        glCallList(2);
    }
    void DrawMoon(void)
    {
        glCallList(3);
    }
    void Lights(int on)
    {
        static GLfloat light_ambient[] = { 0.20f, 0.20f, 0.20f, 0.0f };
        static GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        static GLfloat light_specular[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        //static GLfloat spot0_direction[] = { 4.0f, 0.0f, 0.0f };

        if (on) {

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            static GLfloat spot0_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, spot0_position);

            static GLfloat spot0_color[] = { 0.75f, 0.75f, 1.0f, 1.0f }; //White.
            glLightfv(GL_LIGHT0, GL_DIFFUSE, spot0_color);
            glLightfv(GL_LIGHT0, GL_SPECULAR, spot0_color);
            glShadeModel(GL_SMOOTH);

//            glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot0_direction);
//            glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 0.0f);
        }
        else {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
        }
    }
    void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
    {
        const GLdouble pi = 3.1415926535897932384626433832795;
        GLdouble fW, fH;

        //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
        fH = tan(fovY / 360 * pi) * zNear;
        fW = fH * aspect;

        glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    }

public:

    GLvoid resize(GLsizei w, GLsizei h)
    {
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        perspectiveGL(60.0F, (GLfloat)w / (GLfloat)h, 1.0F, 20.0F);

        glTranslatef(0.0F, 0.0F, -10.0F);  /* Move camera back */
        glRotatef(-90.0F, 1.0F, 0.0F, 0.0F); /* Lets look from here. */

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void beginUse(void)
    {
        initializeGL(0, 0);
    }
    void drawScene(void)
    {
        resize(640, 480);
        GLfloat scale = 4.0f;

        day += 1 * scale;
        if (day > 360.0f) day -= 360.0f;

        month += .5f * scale;
        if (month > 360.0f) month -= 360.0f;

        year += 0.041067762f * scale;
        if (year > 360.0f) year -= 360.0f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();

#ifdef LIGHTING
       Lights(0);
#endif
        DrawSun();

        glRotatef(year, 0.0f, 0.0f, 1.0f);      /* Revolve earth around sun  */
#ifdef LIGHTING
              Lights(1);
#endif

        glTranslatef(5.0f, 0.0f, 0.0f);         /* Push earth into its orbit */

        glPushMatrix();
        glRotatef(-year, 0.0f, 0.0f, 1.0f);     /* Tilt the earth */
        glRotatef(23.45f, 0.0f, 1.0f, 0.0f);
        glRotatef(-day, 0.0f, 0.0f, -1.0f);   /* Rotate earth about itself */
        DrawEarth();
        glPopMatrix();

        glRotatef(month, 0.0f, 0.0f, 1.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);
        DrawMoon();

        glPopMatrix();
        glFlush();
        //        SwapBuffers(ghDC);

    }
    void endUse()
    {

    }
};
#endif
