#define _USE_MATH_DEFINES
#include "GL/freeglut.h"
#include <cmath>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

// Globals

const int degreesToRotate = 1;
const int msUntilNextRotation = 10;
const int colorSpeed = 100;
const float color_r_delta = 0.01f;
const float color_g_delta = 0.015f;
const float color_b_delta = 0.020f;
const float rotation_sensitivity = 0.5f;

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<vector<int>>> vecf;

bool currentlyRotating;
float current_x_rotation, current_y_rotation, initial_x_rotation, initial_y_rotation;
bool currentlyUpdatingColors;

float color_r, color_g, color_b;
bool color_r_ascending, color_g_ascending, color_b_ascending;

bool left_mouse_down, right_mouse_down;
int mouse_movement_x, mouse_movement_y;
int mouse_initial_x, mouse_initial_y;
float zoom_sensitivity;
Vector3f camera;
Vector3f object;
float current_degree;

void drawObject();
void updateRotationAngle(int value);
void drawScene();
void drawObjectTriangles();
void updateColors(int status);
void handleMouse(int button, int state, int x, int y);
void handleActiveMouseMovement(int x, int y);
void handleMouseWheel(int button, int dir, int x, int y);
void manualUpdateRotation(float roation_x, float rotation_y);
Vector3f calculateNextPosition(int current_degree);


// You will need more global variables to implement color and position changes
int current_color;
Vector3f position_vector;

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
		currentlyUpdatingColors = !currentlyUpdatingColors;
		if (currentlyUpdatingColors) {
			glutTimerFunc(colorSpeed, updateColors, 1);
		}
		else
		{
			glutTimerFunc(colorSpeed, updateColors, 0);
		}
        break;
	case 'r':
		// start or stop rotating object
		currentlyRotating = !currentlyRotating;
		glutTimerFunc(msUntilNextRotation, updateRotationAngle, degreesToRotate);
		break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

void updateRotationAngle(int degrees_to_rotate)
{
	if (currentlyRotating)
	{
		manualUpdateRotation(0.0f, float(degrees_to_rotate));
		glutTimerFunc(msUntilNextRotation, updateRotationAngle, degreesToRotate);
	}
}

void manualUpdateRotation(float x_rotation, float y_rotation)
{
	current_y_rotation += y_rotation;
	current_x_rotation += x_rotation;
	if (current_y_rotation >= 360)
		current_y_rotation = 360 - current_y_rotation;
	if (current_x_rotation >= 360)
		current_x_rotation = 360 - current_x_rotation;

	glutPostRedisplay();
}

void cameraRotation()
{
	// Y is axis of rotation
	for (int i = 0; i < 360; i++)
	{
		cout << "Current Position: " << camera.x() << "," << camera.y() << "," << camera.z() << endl;
		Vector3f next_position = calculateNextPosition(i);
		cout << "Next Position: " << next_position.x() << "," << next_position.y() << "," << next_position.z() << endl;
		//current_degree += 1;
	}

}

float distanceBetweenPoints(Vector3f a, Vector3f b)
{
	Vector3f difference = b - a;
	return abs(sqrtf(pow(difference.x(), 2) + pow(difference.y(), 2) + pow(difference.z(), 2)));
}

Vector3f calculateNextPosition(int current_degree)
{
	float current_radian = (current_degree + 1) * ((2 * M_PI) / 360);
	cout << "Radian: " << current_radian << endl;
	// ax az bx bz
	// (camera_x, camera_z
	float radius = distanceBetweenPoints(camera, object);
	//cout << "distanceBetweenPoints: " << radius << endl;
	float x = radius*cos(current_radian) + object.x();
	float y = object.y();
	float z = radius*sin(current_radian) + object.z();
	return Vector3f(x, y, z);
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
	{
    case GLUT_KEY_UP:
        // add code to change light position
		position_vector[1] += 0.5f;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		position_vector[1] -= 0.5f;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		position_vector[0] -= 0.5f;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		position_vector[0] += 0.5f;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

void handleMouse(int button, int state, int x, int y)
{
	//cout << "Received Mouse Callback: " << endl << "\tButton: " << button << " State: " << state << " X: " << x << " Y: " << y << endl;
	left_mouse_down = button == GLUT_LEFT_BUTTON && state == GLUT_DOWN;
	right_mouse_down = button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN;
	
	if (left_mouse_down)
	{
		// Store initial values for x & y in the window so we can do relative position calculations while mouse is still down.
		mouse_initial_x = x;
		mouse_initial_y = y;
		initial_x_rotation = current_x_rotation;
		initial_y_rotation = current_y_rotation;
	}
	else if (right_mouse_down)
	{
		cameraRotation();
	}
	else
	{
		mouse_initial_x = 0;
		mouse_initial_y = 0;
		initial_x_rotation = 0;
		initial_y_rotation = 0;
	}
	

}

void handleActiveMouseMovement(int x, int y)
{
	//cout << "Active Mouse Movement detected:" << endl << "\t X: " << x << " Y: " << y << endl;
	if (left_mouse_down)
	{
		// Rotate Camera around object
		// Half of window width = 180 degree rotation
		// Full Rotation = Full Width = 360 degrees
		// Positive Rotation = Clockwise Rotation
		// Current x - initial x = delta x
		//    -10 - 10 = -20
		//     300 - 150 = 150
		// delta x / screen width 
		//    -20 / 400 = -.05
		//    150 / 400 = .375
		// delta x * 360 = degrees to rotate
		//    -.05 * 360 = -18 degrees 
		//    .375 * 360 = 135 degrees
		// degrees from initial rotation - current rotated amount = degrees to rotate now
		      
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		//cout <<"Current position: "<< x << "," << y << endl;
		//cout << "Previous Positions: "<<mouse_initial_x << "," << mouse_initial_y << endl;
		//cout << "Deltas: " << x - mouse_initial_x << "," << y - mouse_initial_y << endl;
		initial_x_rotation = current_x_rotation;
		initial_y_rotation = current_y_rotation;
		float total_y_degrees = (float((x - mouse_initial_x)) / width) * 360;
		float total_x_degrees = (float((y - mouse_initial_y)) / height) * 360;
		//cout << total_x_degrees << endl;
		//cout << total_y_degrees << endl;
		float x_to_rotate = total_x_degrees - initial_x_rotation;
		float y_to_rotate = total_y_degrees - initial_y_rotation;
		manualUpdateRotation(x_to_rotate*.05f, y_to_rotate*.05f);

	}
}

void handleMouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		// Zoom in, if we're not too close.
		if (camera.z() - zoom_sensitivity >= 2.0f)
		{
			camera.z() -= zoom_sensitivity;
			glutPostRedisplay();
		}
			
	}
	else
	{
		// Zoom out, if we're not too far.
		if (camera.z() + zoom_sensitivity <= 15.0f)
		{
			camera.z() += zoom_sensitivity;
			glutPostRedisplay();
		}
			
	}
	return;
}


// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.

    gluLookAt(camera.x(), camera.y(), camera.z(),
              object.x(), object.y(), object.z(),
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };

	GLfloat color[4] = { color_r, color_g, color_b, 1.0f };
    
	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {position_vector[0], position_vector[1], position_vector[2], 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	drawObject();

}

void drawObject()
{
	glPushMatrix();
	glRotatef(current_x_rotation, 1, 0, 0);
	glRotatef(current_y_rotation, 0, 1, 0);
	//drawObjectTriangles();
	glutSolidTeapot(1.0);
	glPopMatrix();

	// Dump the image to the screen.
	glutSwapBuffers();
}

void drawObjectTriangles()
{
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < vecf.size(); i++) {
		vector<vector<int>> &v = vecf[i];
		int _a = v[0][0];
		int _c = v[0][2];
		int _d = v[1][0];
		int _f = v[1][2];
		int _g = v[2][0];
		int _i = v[2][2];

		glNormal3d(vecn[_c - 1][0], vecn[_c - 1][1], vecn[_c - 1][2]);
		glVertex3d(vecv[_a - 1][0], vecv[_a - 1][1], vecv[_a - 1][2]);
		glNormal3d(vecn[_f - 1][0], vecn[_f - 1][1], vecn[_f - 1][2]);
		glVertex3d(vecv[_d - 1][0], vecv[_d - 1][1], vecv[_d - 1][2]);
		glNormal3d(vecn[_i - 1][0], vecn[_i - 1][1], vecn[_i - 1][2]);
		glVertex3d(vecv[_g - 1][0], vecv[_g - 1][1], vecv[_g - 1][2]);
	}
	glEnd();
}


// Initialize OpenGL's rendering modes
void initRendering()
{
	glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
	glEnable(GL_LIGHTING);     // Enable lighting calculations
	glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
	// Always use the largest square viewport possible
	if (w > h) {
		glViewport((w - h) / 2, 0, h, h);
	}
	else {
		glViewport(0, (h - w) / 2, w, w);
	}

	// Set up a perspective view, with square aspect ratio
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 50 degree fov, uniform aspect ratio, near = 1, far = 100
	gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	// load the OBJ file here
	bool end = false;
	string s;
	Vector3f v;
	char buffer[1000];
	while (cin.getline(buffer, 1000))
	{
		stringstream ss(buffer);
		ss >> s;
		if (s == "v")
		{
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		else if (s == "vn") {
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		else if (s == "f") {
			vector<vector<int>> lines;
			vector<string> strings = { "", "","" };
			ss >> strings[0] >> strings[1] >> strings[2];
			vector<int> line = { 0,0,0 };
			for (int i = 0; i < 3; i++)
			{
				int delim1 = strings[i].find("/", 0);
				int delim2 = strings[i].find("/", delim1 + 1);
				int delim3 = delim2 + 1;
				try {
					line[0] = stoi(strings[i].substr(0, delim1));
					line[1] = stoi(strings[i].substr(delim1 + 1, delim2 - (delim1 + 1)));
					line[2] = stoi(strings[i].substr(delim3));
				}
				catch (exception)
				{
					string tmp = strings[i];
					string tmp2 = s;
					char * tmp3 = buffer;
					continue;
				}
				lines.push_back(line);
			}
			vecf.push_back(lines);
		}
	}
}

void updateColors(int status)
{
	if (status == 1)
	{

		if (color_r_ascending)
		{
			if (color_r + color_r_delta >= 1.0f)
			{
				color_r_ascending = false;
				color_r -= color_r_delta;
			}
			else {
				color_r += color_r_delta;
			}

		}
		else
		{
			if (color_r - color_r_delta <= 0.0f)
			{
				color_r_ascending = true;
				color_r += color_r_delta;
			}
			else {
				color_r -= color_r_delta;
			}
		}
		if (color_g_ascending)
		{
			if (color_g + color_g_delta >= 1.0f)
			{
				color_g_ascending = false;
				color_g -= color_g_delta;
			}
			else {
				color_g += color_g_delta;
			}

		}
		else
		{
			if (color_g - color_g_delta <= 0.0f)
			{
				color_g_ascending = true;
				color_g += color_g_delta;
			}
			else {
				color_g -= color_g_delta;
			}
		}
		if (color_b_ascending)
		{
			if (color_b + color_b_delta >= 1.0f)
			{
				color_b_ascending = false;
				color_b -= color_b_delta;
			}
			else {
				color_b += color_b_delta;
			}

		}
		else
		{
			if (color_b - color_b_delta <= 0.0f)
			{
				color_b_ascending = true;
				color_b += color_b_delta;
			}
			else {
				color_b -= color_b_delta;
			}
		}
		glutPostRedisplay();
		glutTimerFunc(colorSpeed, updateColors, 1);
	}
	
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

	current_color = 0;
	position_vector = { 1.0, 1.0, 5.0 };
	currentlyRotating = false;
	currentlyUpdatingColors = false;
	left_mouse_down, right_mouse_down = false;
	color_r, color_g, color_b = 0.5f;
	zoom_sensitivity = 0.5f;
	camera = { 0.0f, 0.0f, 5.0f };
	// x: positive is left
	// y: positive is down
	// z: positive is away from origin
	object = { 0.0f, 0.0f, 0.0f };
	mouse_movement_x, mouse_movement_y = 0;
	current_degree = 270;

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Rendering Tests");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
	glutMouseFunc(handleMouse);  // Handles Mouse Key Down events
	glutMotionFunc(handleActiveMouseMovement); // Tracks mouse movement while any button is down
	glutMouseWheelFunc(handleMouseWheel); //Tracks Mouse Wheel movement for zoom

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
