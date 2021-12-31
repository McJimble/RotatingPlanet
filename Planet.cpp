#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>

static GLUquadric*		quad;					// Quadric instance for sphere
static GLuint			textureName;			// Name the texture data is binded to
static unsigned char*	textureData;			// Raw RGB pixel data from PPM. 
static int				texWidth, texHeight;	// Size of texture of sphere
static float			rotationDeg;			// Degrees of rotation of planet

unsigned char* GetPpmData(const char* filename, int& width, int& height)
{
	FILE* fp;
	unsigned char* imageData;
	char headerData[70];		

	fp = fopen(filename, "r");
	if(fp == NULL) return NULL;

	// Only making this read raw ppm files of 3 channels, so we expect to see P6 first.
	// If we don't see it, don't read.
	fgets(headerData, 70, fp);
	if (memcmp(headerData, "P6", 2))
	{
		fprintf(stderr, "%s: Not a raw PPM file\n", filename);
		return NULL;
	}


	// Read header information from file; don't stop until the 3 expected lines are found.
	int i = 0;
	int d = 0;
	while (i < 3) 
	{
		fgets(headerData, 70, fp);
		if (headerData[0] == '#')		
			continue;
		if (i == 0)
			i += sscanf(headerData, "%d %d %d", &width, &height, &d);
		else if (i == 1)
			i += sscanf(headerData, "%d %d", &height, &d);
		else if (i == 2)
			i += sscanf(headerData, "%d", &d);
	}

	// Get all image data as 1-D array, size width * height * number of channels.
	// Also error check for empty sized image.
	int imgSize = width * height * 3;
	if (imgSize <= 0) return NULL;

	imageData = (unsigned char*)malloc(sizeof(unsigned char) * imgSize);
	fread(imageData, sizeof(unsigned char), imgSize, fp);
	fclose(fp);

	return imageData;
}

void Init()
{
	// Enable opengl features we will use
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	// Bind texture data, and set proper tex/material parameters.
	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	quad = gluNewQuadric();
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// Render sphere with a binded texture and material with proper lighting data.
	GLfloat blueAmbience[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat diffuseMat[] = { 0.1, 0.2, 1.0, 1.0 };
	GLfloat specularMat[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat shininessMat[] = { 5.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, blueAmbience);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMat);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininessMat);

	gluQuadricDrawStyle(quad, GLU_FILL);
	glBindTexture(GL_TEXTURE_2D, textureName);
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);

	glPushMatrix();
	glRotatef(rotationDeg, 0.0, 1.0, 0.0);
	
	gluSphere(quad, 5, 20, 20);
	glPopMatrix();

	// Make camera look at direction.
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glPopMatrix();

	glFlush();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0 * (w / h), 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void Rotate(int val)
{
	rotationDeg += (rotationDeg > 360.0f) ? -360.0f : 2.0f;
	glutPostRedisplay();
	glutTimerFunc(25, Rotate, 0);
}

int main(int argc, char** argv)
{
	textureData = GetPpmData("scuff.ppm", texWidth, texHeight);
	if (textureData == NULL)
	{
		printf("Could not open texture ppm.");
		return -1;
	}

	// Init glut if texture successfully grabbed.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(texWidth, texHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Planet");
	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_FLAT);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	Init();

	glutTimerFunc(25, Rotate, 0);
	glutMainLoop();

	return 0;
}