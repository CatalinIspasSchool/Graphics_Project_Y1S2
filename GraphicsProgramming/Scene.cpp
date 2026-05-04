#include "Scene.h"

// Scene constructor, initilises OpenGL
// You should add further variables to need initilised.
Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;
	initialiseOpenGL();

	// Other OpenGL / render setting should be applied here.
	

	// Initialise scene variables
	
}

void Scene::handleInput(float dt)
{
		//camera movement
	float speed = dt;
	//speed boost on shift
	//if (GetAsyncKeyState(VK_SHIFT) & 0x8000) speed += speed;
	
	// Camera movement
	if (input->isKeyDown('w')) myCamera.moveForward(speed * 10);
	else if (input->isKeyDown('s'))	myCamera.moveForward(-speed * 10);
	if (input->isKeyDown('d')) myCamera.moveRight(speed * 10);
	else if (input->isKeyDown('a')) myCamera.moveRight(-speed * 10);
	if (GetAsyncKeyState(VK_SPACE)) myCamera.moveUp(speed * 10);
	else if (GetAsyncKeyState(VK_CONTROL)) myCamera.moveUp(-speed * 10);

	int mousePos[2] = { input->getMouseX(), input->getMouseY()};
	// Camera rotation
	if (input->isMouseRDown())
	{
		myCamera.turnUp(mousePreviousPos[1] - mousePos[1]);
		myCamera.turnRight(mousePos[0] - mousePreviousPos[0]);
	}
	mousePreviousPos[0] = mousePos[0];
	mousePreviousPos[1] = mousePos[1];

		//light controls
	if (input->isKeyDown('l')) 
	{	
		if (!lightJustChanged) lightState = !lightState;
		lightJustChanged = true;
	}else lightJustChanged = false;
}

void Scene::update(float dt)
{
	// update scene related variables.

	// Calculate FPS for output
	calculateFPS();
}

void Scene::render() {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	//gluLookAt(0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	myCamera.update();
	
	// Render geometry/scene here -------------------------------------

	//Skybox
	glPushMatrix();
		useAmbientOnlyMaterial();
		drawSkybox();
	glPopMatrix();
	//ducks
	glPushMatrix();
		glTranslatef(19, 7.8, 16);
		glRotatef(-90, 0, 1, 0);
		useShinyMaterial();
		duck.Render();
		glPushMatrix();
			glTranslatef(6.5, 1.65, 1.8);
			glScalef(0.03, 0.03, 0.03);
			glRotatef(90, 0, 1, 0);
			glRotatef(-90, 0, 0, 1);
			swordCutter.Render();
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(14, 7, 29);
		glRotatef(90, 0, 1, 0);
		glRotatef(-20, 1, 0, 0);
		useGreenMaterial();
		duck.Render();
		glPushMatrix();
			glTranslatef(6.5, 1.65, 1.8);
			glScalef(0.03, 0.03, 0.03);
			glRotatef(90, 0, 1, 0);
			glRotatef(-90, 0, 0, 1);
			swordEpic.Render();
		glPopMatrix();
	glPopMatrix();

	
	//island
	glPushMatrix();
		useShinyMaterial();
		glBindTexture(GL_TEXTURE_2D, waterTexture);
		proceduralShapes.GenerateWaves(175, 300, 2, 3, 0.004, time);
		glTranslatef(0,-10,0);
		useAmbientOnlyMaterial();
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		proceduralShapes.GenerateHill(50,40,18);
	glPopMatrix();


	//Render light
	if (lightState) placeDayLight();
	else placeNightLight();

	// End render geometry --------------------------------------

	// Render text, should be last object rendered.
	renderTextOutput();
	
	// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
}

void Scene::initialiseOpenGL()
{
	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);			// Cornflour Blue Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// Clear stencil buffer
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Blending function

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	skyboxTexture = SOIL_load_OGL_texture(
		"gfx/new_skybox.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	grassTexture = SOIL_load_OGL_texture(
		"gfx/grass_albedo.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	waterTexture = SOIL_load_OGL_texture(
		"gfx/water.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	duck.Load("models/RubberDuck_Cyan.obj", "models/Texture_RubberDucks.png");
	swordCutter.Load("models/cutter01.obj", "gfx/sword_textures.png");
	swordEpic.Load("models/dagger_epic.obj", "gfx/sword_textures.png");

	glEnable(GL_LIGHTING);
}


void Scene::drawSolar()
{
	glPushMatrix();	//Sun
		glColor3f(1, 1, 0);
		glRotatef(time / 30, 0, 0, 1);
		glTranslatef(7, 0, 0);

		glutWireSphere(3, 20, 10);

		glPushMatrix();	//Planet
			glColor3f(0, 0, 1);
			glRotatef(time / 20, 0, 0, 1);
			glTranslatef(20, 0, 0);

			glutWireSphere(1, 10, 7);

			glPushMatrix();	//Moon1
				glColor3f(0.4, 0.4, 0.4);
				glRotatef(time / 5, 0, 0, 1);
				glTranslatef(3, 0, 0);

				glutWireSphere(0.4, 7, 5);
			glPopMatrix();	//Moon1
			glPushMatrix();	//Moon2
				glColor3f(0.6, 0.6, 0.6);
				glRotatef(time / 8, 0, 1, 0);
				glTranslatef(4.5, 0, 0);
				
				glutWireSphere(0.3, 7, 5);
			glPopMatrix();	//Moon2
		glPopMatrix();	//Planet

		
		glPushMatrix();	//Planet2
			glColor3f(1, 0, 0);
			glRotatef(time / 6, 0, 0, 1);
			glTranslatef(10, 0, 0);

			glutWireSphere(0.9, 10, 7);

			glPushMatrix();	//Moon1
				glColor3f(0.4, 0.4, 0.4);
				glRotatef(time / 3, 0, 0.3, 1);
				glTranslatef(3, 0, 0);

				glutWireSphere(0.4, 7, 5);

				glPushMatrix();	//Moonception
					glColor3f(0.2, 0.2, 0.2);
					glRotatef(time / 5, 0, 0.3, 1);
					glTranslatef(3, 0, 0);

					glutWireSphere(0.2, 7, 5);

				glPopMatrix();	//Moonception
			glPopMatrix();	//Moon1
			glPushMatrix();	//Moon2
				glColor3f(0.6, 0.6, 0.6);
				glRotatef(time / 5, 0, 1, 0.6);
				glTranslatef(4.5, 0, 0);
				
				glutWireSphere(0.3, 7, 5);
			glPopMatrix();	//Moon2
		glPopMatrix();	//Planet2
	glPopMatrix();	//Sun



	//glPopMatrix();
	//
	//glPushMatrix();

	//	//glRotatef(triangleRotation, 0, 0, 1);
	//	glTranslatef(-1.5, 0, 0);
	//	glScalef(2, 2, 1);

	//	glBegin(GL_TRIANGLES);
	//		glVertex3f(0, 0.5, 0);
	//		glVertex3f(0.5, 0, 0);
	//		glVertex3f(-0.5, 0, 0);
	//	glEnd();

	//glPopMatrix();
}

void Scene::drawSkybox()
{
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();	//StartCube

	glTranslatef(myCamera.Position.x, myCamera.Position.y - 0.2, myCamera.Position.z);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture);//Bind our texture
	glColor3f(1.0f, 1.0f, 1.0f);//Make sure the colour is white

	glBegin(GL_QUADS);
	//face 1
	glNormal3f(0.0f, 0.0f, -1.0f);
	//glColor3f(1.f, 0.f, 0.f);
	glTexCoord2f(0.75f, 0.333f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0f, 0.333f);
		glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(1.0f, 0.666f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.75f, 0.666f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

	//face 2
	glNormal3f(1.0f, 0.0f, 0.0f);
	//glColor3f(0.f, 0.f, 1.f);
	glTexCoord2f(0.25f, 0.333f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.0f, 0.333f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.0f, 0.666f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.25f, 0.666f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

	//face 3
	glNormal3f(0.0f, 0.0f, 1.0f);
	//glColor3f(1.f, 0.f, 0.f);
	glTexCoord2f(0.25f, 0.333f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.5f, 0.333f);
		glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.5f, 0.666f);
		glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(0.25f, 0.666f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

	//face 4
	glNormal3f(-1.0f, .0f, 0.0f);
	//glColor3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.5f, 0.333f);
		glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.75f, 0.333f);
		glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.75f, 0.666f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.5f, 0.666f);
		glVertex3f(0.5f, -0.5f, -0.5f);

	//face 5
	glNormal3f(0.0f, -1.0f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.5f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.5f);
	glTexCoord2f(0.5f, 0.333f);
		glVertex3f(0.5f, 0.5f, -0.5f);
	glTexCoord2f(0.25f, 0.333f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

	//face 6
	glNormal3f(0.0f, 1.0f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.25f, 0.666f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.5f, 0.666f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.5f, -0.5f, -0.5f);
	glTexCoord2f(0.25f, 1.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

	glEnd();
	glPopMatrix(); //EndCube

	glEnable(GL_DEPTH_TEST);
}

void Scene::placeDayLight()
{

	GLfloat Light_Ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat Light_Diffuse[] = { 1.f, 0.5f, 0.3f, 0.0f };
	GLfloat Light_DiffuseDirection[] = { 0.f, 1.f, 0.f, 0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, Light_Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, Light_DiffuseDirection);
	glEnable(GL_LIGHT0);


}

void Scene::placeNightLight()
{

	GLfloat Light_Ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat Light_Diffuse[] = { 0.2f, 0.7f, 0.4f, 0.0f };
	GLfloat Light_DiffuseDirection[] = { 0.f, 1.f, 0.f, 0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, Light_Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, Light_DiffuseDirection);
	glEnable(GL_LIGHT0);


}

void Scene::useMatteMaterial()
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_ambient_colour[] = { 2, 2, 2, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess = 16.f;
	GLfloat mat_emission[] = { 0.3, 0.2, 0.2, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_colour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);


}

void Scene::useShinyMaterial()
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_ambient_colour[] = { 1, 1, 1, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess = 16.f;
	GLfloat high_shininess = 128.f;
	GLfloat mat_emission[] = { 0.3, 0.2, 0.2, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_colour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

}

void Scene::useAmbientOnlyMaterial()
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_ambient_colour[] = { 1.5, 1.5, 1.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat low_shininess = 16.f;
	GLfloat high_shininess = 128.f;
	GLfloat mat_emission[] = { 0.3, 0.2, 0.2, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_colour);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, no_mat);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

}

void Scene::useGreenMaterial()
{

	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat mat_ambient_colour[] = { 0.3, 1.5, 0.3, 1.0 };
	GLfloat mat_specular[] = { 0.4, 1.0, 0.4, 1.0 };
	GLfloat low_shininess = 16.f;
	GLfloat high_shininess = 128.f;
	GLfloat mat_emission[] = { 0.3, 0.2, 0.2, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_colour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

}



// Handles the resize of the window. If the window changes size the perspective matrix requires re-calculation to match new window size.
void Scene::resize(int w, int h)
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

// Calculates FPS
void Scene::calculateFPS()
{
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame * 1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}
}

// Compiles standard output text including FPS and current mouse position.
void Scene::renderTextOutput()
{
	// Render current mouse position and frames per second.
	sprintf_s(mouseText, "Mouse: %i, %i", input->getMouseX(), input->getMouseY());
	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
}

// Renders text to screen. Must be called last in render function (before swap buffers)
void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width / (float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}
