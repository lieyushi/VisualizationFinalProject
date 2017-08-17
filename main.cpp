#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "FileLoader.h"
#include "texture.h"
#include "ColorFunc.h"
#include "DrawFunc.h"
#include "PCA_Cluster.h"
#include "Shader.h"
#include <stdlib.h>
#include <time.h>
#include <GL/glui.h>
#include <GL/freeglut.h>
#include <climits>
#include <iomanip>

using namespace std;

#define ESCAPE 0x1b
FileLoader *object = NULL;
clock_t start_, end_;

#ifndef GLUT_WHEEL_UP  
#  define GLUT_WHEEL_UP   3  
#  define GLUT_WHEEL_DOWN 4  
#endif 


int main_window;
int Simulation;
int Measurement;
int Frame = 0;
int DepthCueOn = 0;
int Full;
GLUI *glui;
int Steps = 2;
int ParticleType;
int colorTransfer;
void *font = GLUT_BITMAP_8_BY_13;
int pointSize;
float veloThreshold, densityThreshold, dvThreshold, ftleThreshold, rotationThreshold;
int AxesOn;
int Tracking;
int ParVis;
int Pathline;
int MapTexture;
int Clustering;
int ClusterEnabled;

enum ButtonVals
{
	CB_RESET,
	RESET,
	QUIT,
	CONTINUE,
	STOP
};

enum LeftButton
{
	ROTATE,
	SCALE
};


const int LEFT = 0;
const int MIDDLE = 1;
const int RIGHT = 2;
const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };
const int INIT_WINDOW_SIZE[] = { 1400, 1400};
const float MINSCALE = 0.05f;
const float SCLFACT = 0.005f;
const float ANGFACT = 0.15;
const int TEXT_HEIGHT = 13;
const int& FRAME = 1;

const int& lightNum = 3;
static GLfloat glCameraPosition[3];
static GLfloat glLightPosition[9];
static GLfloat glLightColor[9];
static GLint program;
static Shader s;


GLfloat	RotMatrix[4][4];
float Scale, Scale2;
float Xrot, Yrot;
float TransXYZ[3];	 
int	Xmouse, Ymouse, ActiveButton ;	
int LeftButton;

/* use colorList to store the color */
float *colorList = NULL;
/* use trajectory to store the trajectory data */
float **trajectory = NULL;
float **trajectoryColor = NULL;
float **trajPos = NULL;
vector<int> trajeIndex;
std::vector< std::vector<float> > massCenter;
float *clusterColor = NULL;

vector<int> indices;
GLuint	axesList = -1;
GLuint boxList = -1;
const string& location = "/media/lieyu/Seagate Backup Plus Drive/NumericalAnalysis_FluidSimulation/noSurfaceTension/twoHalf_Merging/sourceData/Frame ";
int beginning, ending;
string fileName;
GLuint colorVBO, indicesVBO, coordinateVBO;

float Extreme[7][2] = 
					{
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN,
						FLT_MAX, FLT_MIN
					};


/*********************************************************************/

void myGlutIdle( void );
void Display();
void initGUI();
void myInit();
void keyboard( unsigned char c, int x, int y );
void reset();
void Buttons( int id );
void Resize( int width, int height );
void myGlutReshape( int x, int y );
void MouseButton( int button, int state, int x, int y );
void MouseMotion(int x, int y);
void timerCB(int millisec);
void drawTextureCarpet(const char* filename);
void getColorMapping(const float& min_, const float& max_, const float& value, float rgb[3]);
void showInfo();
void drawPoints();
void drawParticleType();
void getColorList();
void Sliders( int id );
void getFirst();
void mouseWheel(int button, int dir, int x, int y);
void drawPathlines();
void readTrajectory();
void deleteMemory();
void drawTexture(const int& textureConst);
void performPCAAnalysis();
void drawClustering();
void computeClustering();
void passInShaderAttribute();

int main(int argc, char* argv[])
{
	time(&start_);	

	if(argc < 2)
	{
		beginning = 50;
	}
	else if(argc == 2)
	{
		beginning = atoi(argv[1]);
	}
	else
	{
		perror("Error for argument input!\n");
		exit(1);
	}

	object = new FileLoader();

	getFirst();
	object->readFile(fileName);

	readTrajectory();

	computeClustering();

	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowPosition( 100, 100 );
	glutInitWindowSize( INIT_WINDOW_SIZE[0], INIT_WINDOW_SIZE[1] );
	main_window = glutCreateWindow(  "Position Based Fluids" );

	GLint GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult) 
	{
		printf("ERROR: %s\n",glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	InitLists(axesList, boxList);

	myInit();
	reset();

	glutDisplayFunc( Display );
	glutReshapeFunc( myGlutReshape );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc(mouseWheel);
	glutTimerFunc(33, timerCB, 33);
	glutPassiveMotionFunc( NULL );

	initGUI();
	glutMainLoop();

	glUseProgram(0);
	time(&end_);

	deleteMemory();

	return 0;
}



void myGlutIdle( void )
{
	if(Simulation)
	{
		//sleep(100);
		beginning+=Steps;
		getFirst();
		object->readFile(fileName);
	}

	if ( glutGetWindow() != main_window )
    	glutSetWindow(main_window);

	glutPostRedisplay();
}


void Display()
{
	GLsizei vx, vy, v;	
	GLint xl, yb;		
	GLfloat scale2;		

	glutSetWindow( main_window );
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	vx = glutGet( GLUT_WINDOW_WIDTH );
	vy = glutGet( GLUT_WINDOW_HEIGHT );
	glViewport( 0, 0,  vx,vy);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60., ((float)vx/(float)vy),	0.1, 1000. );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const float& center_x = 1.0*20;
	const float& center_y = 0.0;
	const float& center_z = 3.0*20;

	gluLookAt(-2.5*20, 1.0*20, 3.8*20, 1.0*20, 1.0*20, 3.8*20, 0, 1, 0);

	glCameraPosition[0] = -2.5*20;
	glCameraPosition[1] = 1.0*20;
	glCameraPosition[2] = 3.8*20;

	glTranslatef( 3.3*20, -1.5*20, 1*20);
	glTranslatef( -(GLfloat)TransXYZ[0], -(GLfloat)TransXYZ[1], (GLfloat)TransXYZ[2] );

	glTranslatef( center_x, center_y, center_z);
	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );
	glMultMatrixf( (const GLfloat *) RotMatrix );
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	scale2 = 1. + Scale2;
	if( scale2 < MINSCALE )
		scale2 = MINSCALE;
	glScalef( (GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2 );	
	glTranslatef(-center_x, -center_y, -center_z);

	GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 0.0f};
	GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 0.0f};
	GLfloat light0_position[] = {-20.5f, 50.0f, 60.0f, 0.0f};

	for (int i = 0; i < 3; ++i)
	{
		glLightColor[i] = light0_ambient[i];
		glLightColor[i+3] = light0_diffuse[i];
		glLightPosition[3*i] = 40+5*i;
		glLightPosition[3*i+1] = 100+5*i;
		glLightPosition[3*i] = 80+5*i;
	}
	glLightColor[6] = 0.0, glLightColor[7] = .6, glLightColor[8] = 1.0;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	if(AxesOn)
	{
		glColor3f( 1, 0, 1 );
		glCallList( axesList );
	}

	//passInShaderAttribute();

	drawTexture(MapTexture);
	
	getColorList();

	if(ParVis)
	{
		drawParticleType();
		if(!Tracking)
		{
			(&indices)->clear();
		}
	}
	else if(Pathline)
		drawPathlines();
	else if(ClusterEnabled)
		drawClustering();

	//glUseProgram(0);
	glutSwapBuffers();

	glFlush();
}

void initGUI()
{
	program = glCreateProgram();
	GLUI_Panel *panel, *panel_;
	GLUI_RadioGroup *group;
	GLUI_Rotation *rot;
	GLUI_Translation *trans, *scale;
	char str[128];

	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_LEFT );
	
	/*glutInitWindowPosition( INIT_WINDOW_SIZE[0] + 50, 0 );
	glui = GLUI_Master.create_glui( (char *) "Fluid Simulation Control" );*/


	glui->add_separator();
	panel = glui->add_panel(  "Simulation Operation" );
		panel_ = glui->add_panel_to_panel( panel, "");
		glui->add_button_to_panel( panel_, "Reset", RESET, (GLUI_Update_CB) Buttons );
		glui->add_column_to_panel( panel_, false );
		glui->add_button_to_panel( panel_, "Quit", QUIT, (GLUI_Update_CB) Buttons );
	//glui->add_column_to_panel( panel, false );
		glui->add_checkbox_to_panel( panel, "Run Simulation", &Simulation );
		glui->add_checkbox_to_panel( panel, "Intensity Depth Cue", &DepthCueOn );
		glui->add_checkbox_to_panel( panel, "Fll Boundary", &Full );
		glui->add_checkbox_to_panel( panel, "Axes", &AxesOn );

	//panel = glui->add_panel(  "Fluid Simulation" );
	GLUI_Rollout *rollout = glui->add_rollout(" Colormapping Measurement ", 0);
		group = glui->add_radiogroup_to_panel( rollout, &Measurement );
			glui->add_radiobutton_to_group( group, "Velocity" );
			glui->add_radiobutton_to_group( group, "Density" );
			glui->add_radiobutton_to_group( group, "Neighbor" );
			glui->add_radiobutton_to_group( group, "Speed Gradient Norm" );
			glui->add_radiobutton_to_group( group, "Density Gradient Norm" );
			glui->add_radiobutton_to_group( group, "ftle" );
			glui->add_radiobutton_to_group( group, "Rotation" );

	GLUI_Rollout *rollout_ = glui->add_rollout(" Particle Display Types ", 0);
		group = glui->add_radiogroup_to_panel( rollout_, &ParticleType );
				glui->add_radiobutton_to_group( group, "Points" );
				glui->add_radiobutton_to_group( group, "Spheres" );
				glui->add_radiobutton_to_group( group, "Realistic" );

	panel = glui->add_panel(  "Simulation Parameters" );
		//glui->add_column_to_panel( panel, false );
		GLUI_Spinner *segment_spinner = glui->add_spinner_to_panel( panel, "Steps:", GLUI_SPINNER_INT, &Steps );
 		segment_spinner->set_int_limits( 2, 6 );

 	glui->add_separator();
 	panel = glui->add_panel( "Object Transformation" );
 	panel_ = glui->add_panel_to_panel(panel, "");

		rot = glui->add_rotation_to_panel( panel_, "Rotation", (float *) RotMatrix );
		rot->set_spin( 1.0 );// allow the object to be spun via the glui rotation widget:
	glui->add_column_to_panel( panel_, false );
		scale = glui->add_translation_to_panel( panel_, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.005f );
	glui->add_column_to_panel( panel_, false );
		trans = glui->add_translation_to_panel( panel_, "Trans YZ", GLUI_TRANSLATION_XY, &TransXYZ[2] );
		trans->set_speed( 0.05f );
	glui->add_column_to_panel( panel_, false );
		trans = glui->add_translation_to_panel( panel_, "Trans X",  GLUI_TRANSLATION_Z , &TransXYZ[0] );
		trans->set_speed( 0.05f );	
	glui->add_button_to_panel( panel, "Reset Object Transform", CB_RESET, (GLUI_Update_CB)Buttons);

	
	panel = glui->add_panel( "Visualization Setting" );
	panel_ = glui->add_panel_to_panel( panel, "" );
	glui->add_separator();
		glui->add_column_to_panel( panel_, false );
		rollout = glui->add_rollout_to_panel(panel, " Color transfer ", 0);
			group = glui->add_radiogroup_to_panel( rollout, &colorTransfer );
			glui->add_radiobutton_to_group( group, "NoTransfer" );
			glui->add_radiobutton_to_group( group, "Rainbow" );
			glui->add_radiobutton_to_group( group, "BlueWhiteRed" );
			glui->add_radiobutton_to_group( group, "GrayScale" );
			glui->add_radiobutton_to_group( group, "Heated" );
		glui->add_column_to_panel( panel_, false );
			segment_spinner = glui->add_spinner_to_panel( panel_, "pointSize:", GLUI_SPINNER_INT, &pointSize );
 			segment_spinner->set_int_limits( 1, 5 );

 			segment_spinner = glui->add_spinner_to_panel( panel_, "velocity threshold:", GLUI_SPINNER_FLOAT, &veloThreshold );
 			segment_spinner->set_int_limits( object->limit[0][0], object->limit[0][1]);

 			segment_spinner = glui->add_spinner_to_panel( panel_, "density threshold:", GLUI_SPINNER_FLOAT, &densityThreshold );
 			segment_spinner->set_int_limits( object->limit[1][0], object->limit[1][1] );

 			segment_spinner = glui->add_spinner_to_panel( panel_, "velo_gradient threshold:", GLUI_SPINNER_FLOAT, &dvThreshold );
 			segment_spinner->set_int_limits( log(object->limit[3][0]), log(object->limit[3][1]) );

 			segment_spinner = glui->add_spinner_to_panel( panel_, "ftle percentage:", GLUI_SPINNER_FLOAT, &ftleThreshold );
 			segment_spinner->set_int_limits( 0.0, 1.0 );

 			segment_spinner = glui->add_spinner_to_panel( panel_, "rotation threshold:", GLUI_SPINNER_FLOAT, &rotationThreshold );
 			segment_spinner->set_int_limits( object->limit[6][0], object->limit[6][1] );

 			glui->add_checkbox_to_panel( panel_, "Tracking", &Tracking );

 	panel = glui->add_panel( "Visualization Scenarios" );
 		glui->add_column_to_panel( panel, false );
 		glui->add_checkbox_to_panel( panel, "Particle Visualization", &ParVis );
 		glui->add_column_to_panel( panel, false );
		glui->add_checkbox_to_panel( panel, "Trajectory Visualization", &Pathline );

	panel = glui->add_panel( "Trajectory Clustering" );
	glui->add_column_to_panel( panel, false );
		glui->add_checkbox_to_panel( panel, "Clustering Enabled", &ClusterEnabled );
	rollout_ = glui->add_rollout(" Clustering Methods ", 0);
		group = glui->add_radiogroup_to_panel( rollout_, &Clustering );
				glui->add_radiobutton_to_group( group, "K-means" );
				glui->add_radiobutton_to_group( group, "AHC" );
				glui->add_radiobutton_to_group( group, "GMM-EM" );

	glui->set_main_gfx_window( main_window );

	GLUI_Master.set_glutIdleFunc( myGlutIdle );
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);
}

void myInit()
{
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_CULL_FACE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.3,0.3,0.5);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black and opaque
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL); 

	colorList = new float[object->number*3];
	for (int i = 0; i < object->number; ++i)
	{
	 	memset(&colorList[i*3],0.0,sizeof(float)*3);
	} 

	//program = glCreateProgram();
}


void reset()
{
	Simulation = 0;
	Measurement = 0;
	ParticleType = 0;
	ActiveButton = LEFT;
	colorTransfer = 1;
	LeftButton = ROTATE;
	Scale  = 1.0f;
	Scale2 = 0.0f;
	pointSize = 3.0;
	DepthCueOn = 0;
	Full = 0;
	AxesOn = 0;
	Tracking = 0;
	ParVis = 0;
	Pathline = 0;
	MapTexture = 0;
	Clustering = 0;
	ClusterEnabled = 0;

	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;
	RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;

	veloThreshold = object->limit[0][0];
	densityThreshold = object->limit[1][0]; 
	dvThreshold = log(object->limit[3][0]);
	ftleThreshold = 0.0; 
	rotationThreshold = object->limit[6][0];
}	


void keyboard( unsigned char c, int x, int y )
{

	switch( c )
	{
		case 'c':
		case 'C':
			Buttons(CONTINUE);
			break;

		case 's':
		case 'S':
			Buttons(STOP);
			break;

		case 'r':
		case 'R':
			Buttons(RESET);
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			Buttons(QUIT);
			break;			// happy compiler

		case 'i':
		case 'I':
			MapTexture++;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
			break;


	}

	glui->sync_live();

	glutSetWindow( main_window );
	glutPostRedisplay();
}


void Buttons( int id )
{
	char str[256];
	switch( id )
	{
		case CB_RESET:
			{
				Xrot = Yrot = 0.;
				TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;
				RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
				RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
				RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
				RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
				RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;
			
				glui->sync_live();
				glutSetWindow( main_window );
				glutPostRedisplay();
			}
			break;


		case RESET:
			reset();

			glui->sync_live();
			glutSetWindow( main_window );
			glutPostRedisplay();
			break;

		case QUIT:

			glui->close();
			glutSetWindow( main_window );
			glFinish();
			glutDestroyWindow( main_window );
			exit( 0 );
			break;

		case CONTINUE:
			Simulation = 1;
			glui->sync_live();
			glutSetWindow( main_window );
			glutPostRedisplay();
			break;

		case STOP:
			Simulation = 0;
			glui->sync_live();
			glutSetWindow( main_window );
			glutPostRedisplay();
			break;

		default:
			fprintf( stderr, "Don't know what to do with Button ID %d\n", id );
	}

}


void Resize( int width, int height )
{
	glutSetWindow( main_window );
	glutPostRedisplay();
}


void myGlutReshape(int	x, int y)
{
	/*int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
	glutPostRedisplay();*/

	glViewport(0,0,x, y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective( 60., ((float)x/(float)y),	0.1, 1000. );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void MouseButton( int button, int state, int x, int y )
{
	int b;			// LEFT, MIDDLE, or RIGHT

	switch( button )
	{
	default:
	case GLUT_LEFT_BUTTON:
		b = LEFT;		
		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		
		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		
		break;
	}

	// button down sets the bit, up clears the bit:
	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton = b;
	}
	else
	{
		ActiveButton = -1;
	}
}


void MouseMotion(int x, int y)
{
	int dx, dy;	
	dx = x - Xmouse;	
	dy = y - Ymouse;

	switch(ActiveButton)
	{
	case LEFT:
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
		break;

	case MIDDLE:
		Scale += SCLFACT * (float) ( dx - dy );
		if( Scale < MINSCALE )
			Scale = MINSCALE;
		break;

	case RIGHT:
		TransXYZ[2] += 0.01f*dx;
		TransXYZ[1] += 0.01f*dy;
		break;
	}

	Xmouse = x;		
	Ymouse = y;

	glutSetWindow( main_window );
	glutPostRedisplay();
}

void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}


void drawTextureCarpet(const char* filename)
{
	glEnable(GL_TEXTURE_2D);

	const int& SEGMENT = 30;

	Texture texture;

	GLuint id_texture;

	if(strstr(filename, ".bmp")!=NULL)
	{
		id_texture = texture.LoadBitmap(filename);
	}
	else if(strstr(filename, ".png")!=NULL)
	{
		id_texture = texture.png_texture_load(filename);
	}
	else if(strstr(filename, ".jpg")!=NULL)
	{
		id_texture = texture.jpg_texture_load(filename);
	}
	else
	{
		std::cout << "Error for texture mapping!" << std::endl;
		exit(1);
	}

	float uv_array[4][2] = 
	{
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	};

	if(Full)
	{

		float XZcarpet[2][2] = 
		{
			-6.0*20, 2.4*20,
			-4.0*20, 13.0*20,
		};

		float XYcarpet[2][2] = 
		{
			-6.0*20, 2.4*20,
			-0.1*20, 10.0*20,
		};

		float insideYZ[2][2] =
		{
			-0.1*20, 10.0*20,
			-4.0*20, 13.0*20
		};


		float x_step = (XZcarpet[0][1]-XZcarpet[0][0])/(float)SEGMENT;
		float y_step = (XZcarpet[1][1]-XZcarpet[1][0])/(float)SEGMENT;
		glBindTexture(GL_TEXTURE_2D, id_texture);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < SEGMENT; ++i)
		{
			for(int j = 0; j < SEGMENT; ++j)
			{
				glNormal3f(0,1,0);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*i);

				glTexCoord2fv(&uv_array[1][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*i);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[3][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*i);
			}
		}

		glEnd();

		x_step = (insideYZ[0][1]-insideYZ[0][0])/(float)SEGMENT;
		y_step = (insideYZ[1][1]-insideYZ[1][0])/(float)SEGMENT;
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < SEGMENT; ++i)
		{
			for(int j = 0; j < SEGMENT; ++j)
			{
				glNormal3f(-1,0,0);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*j,insideYZ[1][0]+y_step*i);

				glTexCoord2fv(&uv_array[1][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*(j+1), insideYZ[1][0]+y_step*i);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*(j+1), insideYZ[1][0]+y_step*(i+1));

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*(j+1), insideYZ[1][0]+y_step*(i+1));

				glTexCoord2fv(&uv_array[3][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*j, insideYZ[1][0]+y_step*(i+1));

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(2.4*20, insideYZ[0][0]+x_step*j,insideYZ[1][0]+y_step*i);
			}
		}
		glEnd();


		x_step = (XYcarpet[0][1]-XYcarpet[0][0])/(float)SEGMENT;
		y_step = (XYcarpet[1][1]-XYcarpet[1][0])/(float)SEGMENT;
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < SEGMENT; ++i)
		{
			for(int j = 0; j < SEGMENT; ++j)
			{
				glNormal3f(0,0,1);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j,XYcarpet[1][0]+y_step*i, -4.0*20);

				glTexCoord2fv(&uv_array[1][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*i, -4.0*20);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*(i+1), -4.0*20);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*(i+1), -4.0*20);

				glTexCoord2fv(&uv_array[3][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j, XYcarpet[1][0]+y_step*(i+1), -4.0*20);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j,XYcarpet[1][0]+y_step*i, -4.0*20);
			}
		}
		glEnd();

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < SEGMENT; ++i)
		{
			for(int j = 0; j < SEGMENT; ++j)
			{
				glNormal3f(0,0,-1);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j,XYcarpet[1][0]+y_step*i, 13.0*20);

				glTexCoord2fv(&uv_array[1][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*i, 13.0*20);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*(i+1), 13.0*20);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XYcarpet[0][0]+x_step*(j+1), XYcarpet[1][0]+y_step*(i+1), 13.0*20);

				glTexCoord2fv(&uv_array[3][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j, XYcarpet[1][0]+y_step*(i+1), 13.0*20);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XYcarpet[0][0]+x_step*j,XYcarpet[1][0]+y_step*i, 13.0*20);
			}
		}
		glEnd();
	}

	else
	{
		float XZcarpet[2][2] = 
		{
			-6.0*20, 6.0*20,
			-4.0*20, 13.0*20,
		};

		float x_step = (XZcarpet[0][1]-XZcarpet[0][0])/(float)SEGMENT;
		float y_step = (XZcarpet[1][1]-XZcarpet[1][0])/(float)SEGMENT;
		glBindTexture(GL_TEXTURE_2D, id_texture);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < SEGMENT; ++i)
		{
			for(int j = 0; j < SEGMENT; ++j)
			{
				glNormal3f(0,1,0);

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*i);

				glTexCoord2fv(&uv_array[1][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*i);

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[2][0]);
				glVertex3f(XZcarpet[0][0]+x_step*(j+1), -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[3][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*(i+1));

				glTexCoord2fv(&uv_array[0][0]);
				glVertex3f(XZcarpet[0][0]+x_step*j, -0.1*20, XZcarpet[1][1]-y_step*i);
			}
		}

		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void getColorList()
{
	const vector<Data>& vec = object->dataList;

	const float& ftleMin = object->limit[5][0];
	const float& fixed = 1.0/(object->limit[5][1]-object->limit[5][0]);

	if(!Tracking)
	{	
		for (int i = 0; i < vec.size(); ++i)
		{
			if(vec[i].velo_norm >= veloThreshold && vec[i].density >= densityThreshold && log(vec[i].dv_norm) >= dvThreshold
			   && (vec[i].ftle-ftleMin)*fixed >= ftleThreshold && vec[i].rotation >= rotationThreshold )
			{
				(&indices)->push_back(i);
			}
		}
	}

	//cout << "Selected particle number is: " << indices.size() << endl;

	int *ptr = &(indices[0]);

	if(colorTransfer==0)
	{
		float def_color[] = {0.5, 1.0, 0.0};

	#pragma omp parallel for num_threads(8)
		for (int i = 0; i < indices.size(); ++i)
		{
			memcpy(&colorList[ptr[i]*3], def_color, 3*sizeof(float));		
		}

		return;
	}

	float tempMin, tempMax;

	switch(Measurement)
	{
	default:
	case 0:
		{
			tempMin = object->limit[0][0];
			tempMax = object->limit[0][1]; 

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, vec[ptr[i]].velo_norm, &colorList[ptr[i]*3]);
			}	
		}
		break;


	case 1:
		{
			tempMin = object->limit[1][0];
			tempMax = object->limit[1][1];

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, vec[ptr[i]].density, &colorList[ptr[i]*3]);
			}
		}
		break;

	case 2:
		{
			tempMin = object->limit[2][0];
			tempMax = object->limit[2][1];

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, vec[ptr[i]].neighbor, &colorList[ptr[i]*3]);
			}
		}
		break;

	case 3:
		{
			tempMin = log(object->limit[3][0]);
			tempMax = log(object->limit[3][1]);

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, log(vec[ptr[i]].dv_norm), &colorList[ptr[i]*3]);
			}
		}
		break;

	case 4:
		{
			tempMin = object->limit[4][0];
			tempMax = object->limit[4][1];

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, vec[ptr[i]].den_norm, &colorList[ptr[i]*3]);
			}
		}
		break;

	case 5:
		{
			tempMin = object->limit[5][0];
			tempMax = object->limit[5][1];

 		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax/2.0, vec[ptr[i]].ftle, &colorList[ptr[i]*3]);
			}
		}
		break;

	case 6:
		{
			tempMin = object->limit[6][0];
			tempMax = object->limit[6][1]; 

		#pragma omp parallel for num_threads(8)
			for (int i = 0; i < indices.size(); ++i)
			{
				getColorMapping(tempMin, tempMax, vec[ptr[i]].rotation, &colorList[ptr[i]*3]);
			}
		}
		break;
	}

	const int& measurePos = 3+Measurement;

#pragma omp parallel for num_threads(8)	
	for (int i = 0; i < FRAME; ++i)
	{
		for (int j = 0; j < object->number; ++j)
		{
			getColorMapping(Extreme[Measurement][0], Extreme[Measurement][1], trajectory[j][10*i+measurePos], &trajectoryColor[j][i*3]);
		}
	}
}



void getColorMapping(const float& min_, const float& max_, const float& value, float rgb[3])
{
	switch(colorTransfer)
	{
	case 1:
		ColorRainbow(min_, max_, value, rgb);
		break;

	case 2:
		ColorBlueWhiteRed(min_, max_, value, rgb);
		break;

	case 3:
		ColorGrayScale(min_, max_, value, rgb);
		break;

	case 4:
		ColorHeated(min_, max_, value, rgb);
		break;
	}
}

void drawParticleType()
{
	switch(ParticleType)
	{
	case 0:
		drawPoints();
		break;

	/*case 1:
		drawSpheres();
		break;

	case 2:
		drawRealistic();
		break;
	*/
	}
}

void drawPoints()
{
	//GLuint colorVBO, indicesVBO, coordinateVBO;

	/*glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, object->dataList.size()*3*sizeof(GL_FLOAT), colorList, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GL_UNSIGNED_INT), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &coordinateVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coordinateVBO);
	glBufferData(GL_ARRAY_BUFFER, object->dataList.size()*sizeof(Data), &object->dataList[0].position.x, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coordinateVBO);

	glColorPointer(3, GL_FLOAT, 0, 0);
    glVertexPointer( 3, GL_FLOAT, (GLsizei)sizeof(Data), (GLubyte*)(sizeof(GL_FLOAT)*3*object->dataList.size()));

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    glPointSize(pointSize);
    glDrawElements(GL_POINTS, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );*/


	glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    glColorPointer(3, GL_FLOAT, 0, colorList);
    glVertexPointer( 3, GL_FLOAT, (GLsizei)sizeof(Data), &object->dataList[0].position.x);

    glPointSize(pointSize);
    glDrawElements(GL_POINTS, (GLsizei)indices.size(), GL_UNSIGNED_INT, &(indices[0]));

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );

}



void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, INIT_WINDOW_SIZE[0], 0, INIT_WINDOW_SIZE[1]); // set to orthogonal projection

    float color[4] = {0, 0, 1, 1};

    stringstream ss;
    ss << std::fixed << std::setprecision(3);

    ss << "Max Particles: " << object->number << ends;
    drawString(ss.str().c_str(), INIT_WINDOW_SIZE[0]-10, INIT_WINDOW_SIZE[1]-TEXT_HEIGHT, color, font);
    ss.str("");

    ss << "Max Elements Indices: " << object->number << ends;
    drawString(ss.str().c_str(), INIT_WINDOW_SIZE[0]-10, INIT_WINDOW_SIZE[1]-(2*TEXT_HEIGHT), color, font);
    ss.str("");

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glLoadIdentity();

    glPopMatrix();                   // restore to previous modelview matrix
}


void getFirst()
{
	stringstream ss;
	ss << location << beginning << string(".txt");
	fileName = ss.str();
	ss.clear();
}


void mouseWheel(int button, int state, int x, int y)
{
	if(state > 0)
	{
		Scale2 += SCLFACT*2.0;
	}
	else
	{
		Scale2 -= SCLFACT*2.0;
	}
}


void drawPathlines()
{

	glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    for (int i = 0; i < object->number; ++i)
    {
    	glColorPointer(3, GL_FLOAT, 0, &(trajectoryColor[i][0]));
	    glVertexPointer( 3, GL_FLOAT, 0, &trajPos[i][0]);

	    glDrawElements(GL_LINE_STRIP, (GLsizei)trajeIndex.size(), GL_UNSIGNED_INT, &(trajeIndex[0]));
    }

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
}


void readTrajectory()
{
	for (int i = 0; i < FRAME; ++i)
	{
		trajeIndex.push_back(i);
	}

	trajectory = new float*[object->number];
	trajPos = new float*[object->number];
	trajectoryColor = new float*[object->number];	

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < object->number; ++i)
	{
		trajectory[i] = new float[10*FRAME];
		trajectoryColor[i] = new float[3*FRAME];
		trajPos[i] = new float[3*FRAME];
	}

	for (int i = 0; i < FRAME; ++i)
	{
		stringstream ss;
		ss << location << (beginning+i) << string(".txt");
		string strTemp = ss.str();
		ss.clear();
		FileLoader *tempFile = new FileLoader();
		tempFile->readFile(strTemp);

		const std::vector<Data>& tempDataList = tempFile->dataList;

	#pragma omp parallel for num_threads(8)
		for (int j = 0; j < tempFile->number; ++j)
		{
			const Data& tempData = tempDataList[j];
			trajectory[j][i*10] = tempData.position.x;
			trajectory[j][i*10+1] = tempData.position.y;
			trajectory[j][i*10+2] = tempData.position.z;
			trajectory[j][i*10+3] = tempData.velo_norm;
			trajectory[j][i*10+4] = tempData.density;
			trajectory[j][i*10+5] = tempData.neighbor;
			trajectory[j][i*10+6] = tempData.dv_norm;
			trajectory[j][i*10+7] = tempData.den_norm;
			trajectory[j][i*10+8] = tempData.ftle;
			trajectory[j][i*10+9] = tempData.rotation;
			memcpy(&trajPos[j][i*3], &trajectory[j][i*10],3*sizeof(float));
		}

		for (int j = 0; j < 7; ++j)
		{
			if(Extreme[j][0]>tempFile->limit[j][0])
				Extreme[j][0] = tempFile->limit[j][0];
			if(Extreme[j][1]<tempFile->limit[j][1])
				Extreme[j][1] = tempFile->limit[j][1];
		}

		if(tempFile!=NULL)
		{
			delete tempFile;
			tempFile = NULL;
		}

		std::cout << "File " << (beginning+i) << " has been loaded!" << std::endl;
	}
}


void deleteMemory()
{
	if(colorList!=NULL)
	{
		delete[] colorList;
	}

	for (int i = 0; i < object->number; ++i)
	{
		delete[] trajectory[i];
		delete[] trajectoryColor[i];
		delete[] trajPos[i];
	}

	if(trajectory!=NULL)
	{
		delete[] trajectory;
		trajectory = NULL;
	}

	if(trajectoryColor!=NULL)
	{
		delete[] trajectoryColor;
		trajectoryColor = NULL;
	}

	if(trajPos!=NULL)
	{
		delete[] trajPos;
	}

	if(!indices.empty())
	{
		(&indices)->clear();
	}
	if(object!=NULL)
	{
		delete object;
	}
	if(clusterColor!=NULL)
	{
		delete[] clusterColor;
	}
}

void drawTexture(const int& textureConst)
{
	switch(textureConst%10)
	{
	case 0:
		drawTextureCarpet("Figure/framing.bmp");
		break;

	case 1:
		drawTextureCarpet("Figure/texture.png");
		break;

	case 2:
		drawTextureCarpet("Figure/wall.png");
		break;

	case 3:
		drawTextureCarpet("Figure/carpet.png");
		break;	

	case 4:
		drawTextureCarpet("Figure/carpet1.png");
		break;	

	case 5:
		drawTextureCarpet("Figure/images.png");
		break;

	case 6:
		drawTextureCarpet("Figure/white1.png");
		break;

	case 7:
		drawTextureCarpet("Figure/white2.png");
		break;

	case 8:
		drawTextureCarpet("Figure/white3.png");
		break;

	case 9:
		drawTextureCarpet("Figure/white4.png");
		break;
	}
}


void drawClustering()
{
    glEnableClientState( GL_VERTEX_ARRAY );
    //glEnableClientState( GL_COLOR_ARRAY );
	for (int i = 0; i < massCenter.size(); ++i)
	{
		glColor3fv(&clusterColor[3*i]);
	    glVertexPointer( 3, GL_FLOAT, 0, &(massCenter[i][0]));

	    glDrawElements(GL_LINE_STRIP, (GLsizei)trajeIndex.size(), GL_UNSIGNED_INT, &(trajeIndex[0]));
	}
	glDisableClientState( GL_VERTEX_ARRAY );
}

void computeClustering()
{
	PCA_Cluster::performPCA_Clustering(trajPos, object->number, FRAME*3, massCenter);
	const int& clusterNum = massCenter.size();

	clusterColor = new float[3*clusterNum];
	srand (time(NULL));
	for (int i = 0; i < clusterNum; ++i)
	{
		clusterColor[i*3] = (float)((double) rand() / (RAND_MAX));
		clusterColor[i*3+1] = (float)((double) rand() / (RAND_MAX));
		clusterColor[i*3+2] = (float)((double) rand() / (RAND_MAX));
	}
}

void passInShaderAttribute()
{
	GLint result;
	//program = glCreateProgram();
	s = Shader();
    s.shaderAttachFromFile(program, GL_VERTEX_SHADER, "shader/vertex.glsl");
    s.shaderAttachFromFile(program, GL_FRAGMENT_SHADER, "shader/fragment.glsl");

    /* link the program and make sure that there were no errors */
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) 
    {
        GLint length;
        char *log;

        /* get the program info log */
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetProgramInfoLog(program, length, &result, log);

        /* print an error message and the info log */
        fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
        free(log);

        /* delete the program */
        glDeleteProgram(program);
        program = 0;
    }
	GLint g_programCameraPosition = glGetUniformLocation(program,"cameraPosition");
	GLint g_programLightPosition = glGetUniformLocation(program,"lightPosition");
	GLint g_programLightColor = glGetUniformLocation(program,"lightColor");

	glUseProgram(program);
	glUniform3fv(g_programCameraPosition, 1, glCameraPosition);
	glUniform3fv(g_programLightPosition, 3, glLightPosition);
	glUniform3fv(g_programLightColor, 3, glLightColor);

}