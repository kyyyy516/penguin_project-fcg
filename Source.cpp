#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include "imageloader.h"
#include "imageloader.cpp"

float angle = 0.0f;         // Current rotation angle of the camera direction around y-axis
float lx = 0.0f, lz = -1.0f; // Components of the camera's direction vector (lx, 0, lz)
float x = 0.0f, y = -0.60f, z = 2.0f;  // Camera position (x, y, z)
float deltaAngle = 0.0f;     // Incremental change in camera direction angle on mouse movement
float deltaMove = 0;         // Incremental change in camera position due to user input
int xOrigin = -1;            // Last known X-coordinate of the mouse pointer (used for mouse movement calculations)

// Variables to control penguin animation
float headAngle = 0.0f; // Rotation angle of the head
float rightWingAngle = 0.0f; // Flapping angle of the right wing
float leftFootSwingAngle = 0.0f; // Swinging angle of the left foot
float rightFootSwingAngle = 0.0f; // Swinging angle of the right foot
bool rightWingUp = true; // Direction control for right wing flapping
bool leftFootSwingForward = true; // Direction control for left foot swinging
bool rightFootSwingForward = false; // Direction control for right foot swinging

float lightIntensity = 1.0f; // Light intensity for day-night transition

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1; // Prevent division by zero if the window is too short

    float ratio = w * 1.0 / h; // Calculate the aspect ratio of the window

    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix
    glLoadIdentity(); // Reset matrix

    glViewport(0, 0, w, h); // Set the viewport to be the entire window

    gluPerspective(60.0f, ratio, 0.1f, 100.0f); // Set the perspective (viewing angle, aspect ratio, near clipping plane, far clipping plane)

    glMatrixMode(GL_MODELVIEW); // Switch back to the modelview matrix
    glLoadIdentity(); // Reset matrix
}

GLuint skyTextureID; // Texture ID for the sky

void loadSkyTexture()
{
    // Load sky texture image
    Image* skyTexture = loadBMP("C:\\zmisc\\sky.bmp"); // Load BMP image file
    if (!skyTexture)
    {
        std::cerr << "Error loading sky texture image file." << std::endl; // Print error message if texture loading fails
        return;
    }

    glGenTextures(1, &skyTextureID); // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, skyTextureID); // Bind the texture ID

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Set magnification filter to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set minification filter to linear
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, skyTexture->width, skyTexture->height,
        0, GL_RGB, GL_UNSIGNED_BYTE, skyTexture->pixels); // Specify the texture image
}

GLuint textureID; // Texture ID for general texture

void loadTexture()
{
    // Load texture image
    Image* textureImage = loadBMP("C:\\zmisc\\ice.bmp"); // Load BMP image file
    if (!textureImage)
    {
        std::cerr << "Error loading texture image file." << std::endl; // Print error message if texture loading fails
        return;
    }

    glGenTextures(1, &textureID); // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the texture ID

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set minification filter to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Set magnification filter to linear
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage->width, textureImage->height,
        0, GL_RGB, GL_UNSIGNED_BYTE, textureImage->pixels); // Specify the texture image
}

void drawPenguin()
{
    // Material properties for the penguin
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };    // Ambient material color (r, g, b, alpha)
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };    // Diffuse material color (r, g, b, alpha)
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // Specular material color (r, g, b, alpha)
    GLfloat mat_shininess[] = { 50.0f };                   // Shininess of the material

    // Set the material properties for the front-facing polygons
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);       // Set ambient reflection properties for front faces
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);       // Set diffuse reflection properties for front faces
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);     // Set specular reflection properties for front faces
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);   // Set shininess for front faces

    //Draw Body
    glPushMatrix(); // Start of root (Penguin's body)
    glTranslatef(0.0f, -0.8f, -1.0f); // Translate to the penguin's position
    glRotatef(angle, 0.0f, 1.0f, 0.0f); // Rotate around the y-axis
    glColor3f(0.7, 0.7, 0.7); // Set color to light gray
    glutSolidSphere(0.16f, 50, 50); // Draw the body sphere

    // Draw Head
    glPushMatrix(); // Start of 1st level parent (Head)
    glColor3f(0.44, 0.44, 0.44); // Set color to dark gray
    glTranslatef(0.0f, 0.15f, 0.0f); // Translate to the head's position
    glRotatef(headAngle, 0.0f, 1.0f, 0.0f); // Rotate the head based on headAngle
    glutSolidSphere(0.15f, 50, 50); // Draw the head sphere

    // Draw Cheek
    glPushMatrix(); // Start of child node of head (Cheek)
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glTranslatef(0.0f, -0.03f, 0.095f); // Translate to the cheek's position
    glScalef(1.8f, 0.8f, 1.05f); // Scale to make an oval shape
    glutSolidSphere(0.06f, 200, 200); // Draw the cheek
    glPopMatrix(); // End of child node of head (Cheek)
     
    // Draw Face
    glPushMatrix(); // Start of child node of head (Face)
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glTranslatef(-0.04f, 0.0f, 0.075f); // Translate to the face's position
    glutSolidSphere(0.085f, 200, 200); // Draw the face
    glTranslatef(0.08f, 0.0f, 0.0f); // Translate to draw the second half of the face
    glutSolidSphere(0.085f, 200, 200); // Draw the second half of the face
    glPopMatrix(); // End of child node of head (Face)

    // Draw Eyes
    glPushMatrix(); // Start of child node of head (Eyes)
    glColor3f(0.43, 0.43, 0.43); // Set color to dark gray
    glTranslatef(-0.05f, 0.0f, 0.11f); // Translate to the eye's position
    glutSolidSphere(0.051f, 200, 200); // Draw the left eye
    glTranslatef(0.10f, 0.0f, 0.0f); // Translate to draw the right eye
    glutSolidSphere(0.051f, 200, 200); // Draw the right eye
    glPopMatrix(); // End of child node of head (Eyes)

    glPushMatrix(); // Start of child node of head (Eye shine)
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glTranslatef(-0.07f, 0.005f, 0.15f); // Translate to the eye shine's position
    glutSolidSphere(0.010f, 200, 200); // Draw the left eye shine
    glTranslatef(0.122f, 0.0f, 0.0f); // Translate to draw the right eye shine
    glutSolidSphere(0.013f, 200, 200); // Draw the right eye shine
    glPopMatrix(); // End of child node of head (Eye shine)

    // Draw Beak
    glPushMatrix(); // Start of child node of head (Beak)
    glColor3f(0.99, 0.97, 0.42); // Set color to yellowish
    glTranslatef(0.0f, -0.01f, 0.15f); // Translate to the beak's position
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // Rotate the beak
    glScalef(1.0f, 0.5f, 1.0f); // Scale to make the cone more flat
    glutSolidCone(0.025f, 0.04f, 20, 2); // Draw the beak
    glPopMatrix(); // End of child node of head (Beak)

    // Draw Blushes
    glPushMatrix(); // Start of child node of head (Blushes)
    glColor3f(0.99, 0.77, 0.69); // Set color to pinkish
    glTranslatef(-0.05f, -0.03f, 0.13f); // Translate to the blushes' position
    glScalef(1.5f, 0.8f, 0.8f); // Scale to make a horizontal oval
    glutSolidSphere(0.027f, 200, 200); // Draw the left blush
    glTranslatef(0.10f / 1.5f, 0.0f, 0.0f); // Translate to draw the right blush, adjusted for scaling
    glutSolidSphere(0.027f, 100, 100); // Draw the right blush
    glPopMatrix(); // End of child node of head (Blushes)

    glPopMatrix(); // End of 1st level parent (Head)

    // Draw Right Wing
    glPushMatrix(); // Start of 1st level parent (Right Wing)
    glColor3f(0.7, 0.7, 0.7); // Set color to light gray
    glTranslatef(0.1f, 0.07f, 0.0f); // Translate to the right wing's position
    glRotatef(120.0f, 1.0f, 0.0f, 0.0f); // Rotate the wing to make it hang down
    glRotatef(-30.0f, 0.0f, 1.0f, 0.0f); // Rotate the wing to align it with the body
    glRotatef(rightWingAngle, 0.0f, 1.0f, 0.0f); // Rotate the wing for flapping movement
    glScalef(0.2f, 0.05f, 0.02f); // Scale to make a flat, elongated shape
    glutSolidSphere(0.8f, 100, 100); // Draw the wing as a flattened sphere (ellipsoid)

    // Draw Ice Cream Cone
    glPushMatrix(); // Start of child node of right wing (Ice cream cone)
    glScalef(5.0f, 20.0f, 50.0f); // Reset the scaling effect by applying the inverse scaling
    glTranslatef(0.12f, -0.03f, -0.065f); // Adjust the position of the cone (right/left, depth, up/down)
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f); // Rotate the cone for proper orientation

    glRotatef(10.0f, 0.0f, 1.0f, 0.0f); // Rotate the cone for proper orientation
    glColor3f(0.95, 0.85, 0.59); // Set color to light brown
    glutSolidCone(0.025f, 0.07f, 20, 2); // Draw the cone

    // Draw Ice Cream Scoop
    glColor3f(0.99, 0.82, 0.67); // Set color to light cream
    glTranslatef(0.0f, 0.0f, -0.01f); // Position the ice cream on top of the cone
    glutSolidSphere(0.025f, 100, 100); // Draw the ice cream as a small sphere

    // Add a second scoop
    glColor3f(0.82, 0.95, 0.71); // Set color to white
    glTranslatef(0.0f, 0.0f, -0.03f); // Position the second scoop on top of the first
    glutSolidSphere(0.02f, 100, 100); // Draw the second ice cream scoop

    glPopMatrix(); // // End of child node of right wing (Ice cream cone)
    glPopMatrix(); // End of 1st level parent (Right Wing)

    // Draw Left Wing
    glPushMatrix(); // Start of 1st level parent (Left Wing)
    glColor3f(0.7, 0.7, 0.7); // Set color to light gray
    glTranslatef(-0.1f, 0.07f, 0.0f); // Translate to the left wing's position
    glRotatef(130.0f, 1.0f, 0.0f, 0.0f); // Rotate the wing to make it hang down
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f); // Rotate the wing to align it with the body
    glScalef(0.2f, 0.05f, 0.02f); // Scale to make a flat, elongated shape
    glutSolidSphere(0.8f, 100, 100); // Draw the wing as a flattened sphere (ellipsoid)
    glPopMatrix(); // End of 1st level parent (Left Wing)

    // Draw Right Foot
    glPushMatrix(); // Start of 1st level parent (Right Foot)
    glColor3f(0.46, 0.46, 0.46); // Set color to dark gray
    glTranslatef(0.05f, -0.15f, 0.0f); // Translate to the right foot's position
    glRotatef(-90.0f + leftFootSwingAngle, 1.0f, 0.0f, 0.0f); // Rotate the foot based on leftFootSwingAngle
    glScalef(0.05f, 0.05f, 0.02f); // Scale to make a flat, elongated shape
    glutSolidSphere(1.0f, 100, 100); // Draw the foot as a flattened sphere (ellipsoid)
    glPopMatrix(); // End of 1st level parent (Right Foot)

    // Draw Left Foot
    glPushMatrix();  // Start of 1st level parent (Left Foot)
    glColor3f(0.46, 0.46, 0.46); // Set color to dark gray
    glTranslatef(-0.05f, -0.15f, 0.0f); // Translate to the left foot's position
    glRotatef(-90.0f + rightFootSwingAngle, 1.0f, 0.0f, 0.0f); // Rotate the foot based on rightFootSwingAngle
    glScalef(0.05f, 0.05f, 0.02f); // Scale to make a flat, elongated shape
    glutSolidSphere(1.0f, 100, 100); // Draw the foot as a flattened sphere (ellipsoid)
    glPopMatrix();  // End of 1st level parent (Left Foot)

    glPopMatrix(); //End of root(Penguin's body)
}

void update(int value)
{
    // Update the head angle within the range of -90 to 90 degrees
    if (headAngle < -90.0f)
        headAngle = -90.0f;
    else if (headAngle > 90.0f)
        headAngle = 90.0f;

    // Update the right wing flapping angle within the range of -30 to 30 degrees
    if (rightWingAngle < -10.0f)
        rightWingUp = true;
    else if (rightWingAngle > 30.0f)
        rightWingUp = false;

    // Adjust the right wing flapping direction based on the current angle
    if (rightWingUp)
        rightWingAngle += 1.0f;
    else
        rightWingAngle -= 1.0f;

    // Update the swinging angle of the left foot
    if (leftFootSwingAngle < -10.0f)
        leftFootSwingForward = true;
    else if (leftFootSwingAngle > 10.0f)
        leftFootSwingForward = false;

    // Adjust the swinging direction of the left foot based on the current angle
    if (leftFootSwingForward)
        leftFootSwingAngle += 1.0f;
    else
        leftFootSwingAngle -= 1.0f;

    // Update the swinging angle of the right foot
    if (rightFootSwingAngle < -10.0f)
        rightFootSwingForward = true;
    else if (rightFootSwingAngle > 10.0f)
        rightFootSwingForward = false;

    // Adjust the swinging direction of the right foot based on the current angle
    if (rightFootSwingForward)
        rightFootSwingAngle += 1.0f;
    else
        rightFootSwingAngle -= 1.0f;

    glutPostRedisplay();
    glutTimerFunc(25, update, 0); // Call the update function after 25 milliseconds
}

// Function to compute the new position of the camera based on movement
void computePos(float deltaMove)
{
    x += deltaMove * lx * 0.1f; // Update x position based on movement direction and deltaMove
    z += deltaMove * lz * 0.1f; // Update z position based on movement direction and deltaMove
}

// Define global variables for controlling the day-night cycle
float ambientIntensity = 0.2f; // Initial ambient light intensity

// Function to render the scene
void renderScene(void)
{
    // If there is movement, update the camera position
    if (deltaMove)
        computePos(deltaMove);

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Set the camera position and orientation
    gluLookAt(
        x, y, z,           // Camera position (x, y, z)
        x + lx, y, z + lz, // Look-at position (center of the screen)
        0.0f, 1.0f, 0.0f   // Up vector (defines the orientation of the camera)
    );

    // Draw the sky sphere
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); // Translate to the camera position
    glBindTexture(GL_TEXTURE_2D, skyTextureID); // Bind the sky texture
    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white (for texture)

    const float radius = 50.0f; // Radius of the sky sphere (adjust as necessary)

    // Draw the sphere
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE); // Enable texture coordinates
    gluSphere(quadric, radius, 50, 50); // Draw the sphere

    gluDeleteQuadric(quadric); // Free the quadric

    glDisable(GL_TEXTURE_2D); // Disable 2D texturing
    glPopMatrix();

    // Draw the Antarctic landscape with texture
    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind the landscape texture

    glBegin(GL_QUADS); // Begin drawing quadrilaterals
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -1.0f, -10.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f, -1.0f, 10.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(10.0f, -1.0f, 10.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(10.0f, -1.0f, -10.0f);
    glEnd(); // End drawing quadrilaterals

    glDisable(GL_TEXTURE_2D); // Disable 2D texturing

    // Enable lighting and set up the light source
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_LIGHT0); // Enable light source 0

    // Set the light position (simulating a low sun angle)
    GLfloat light_position[] = { -1.0f, 0.0f, 0.0f, 1.0f }; // Directional light from low on the horizon
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Adjust light intensity based on the time of day
    GLfloat ambientLight[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0f }; // Ambient light color
    GLfloat diffuseLight[] = { 0.8f, 0.7f, 0.6f, 1.0f }; // Diffuse light color
    GLfloat specularLight[] = { 0.6f, 0.6f, 0.6f, 1.0f }; // Specular light color

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight); // Set ambient light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight); // Set diffuse light
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight); // Set specular light

    // Draw penguin animation
    drawPenguin(); // Call function to draw the penguin animation

    // Swap buffers to display the rendered scene
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
    if (key == 'd') // Rotate to the right when the 'd' key is pressed
        angle += 1.0f;
    else if (key == 'a') // Rotate to the left when the 'a' key is pressed
        angle -= 1.0f;
    else if (key == 'h') // Rotate the head to the right when the 'h' key is pressed
        headAngle += 5.0f;
    else if (key == 'j') // Rotate the head to the left when the 'j' key is pressed
        headAngle -= 5.0f;
}

// Function to process special key events
void processSpecialKeys(int key, int xx, int yy)
{
    // Fraction to determine the amount of movement or rotation per key press
    float fraction = 0.1f;

    // Switch case to handle different special key presses
    switch (key)
    {
        // Case for the left arrow key
    case GLUT_KEY_LEFT:
        angle -= 0.01f; // Decrease the angle to turn left
        lx = sin(angle); // Update the look direction along the x-axis
        lz = -cos(angle); // Update the look direction along the z-axis
        break;

        // Case for the right arrow key
    case GLUT_KEY_RIGHT:
        angle += 0.01f; // Increase the angle to turn right
        lx = sin(angle); // Update the look direction along the x-axis
        lz = -cos(angle); // Update the look direction along the z-axis
        break;

        // Case for the up arrow key
    case GLUT_KEY_UP:
        x += lx * fraction; // Move forward in the x direction
        z += lz * fraction; // Move forward in the z direction
        break;

        // Case for the down arrow key
    case GLUT_KEY_DOWN:
        x -= lx * fraction; // Move backward in the x direction
        z -= lz * fraction; // Move backward in the z direction
        break;

        // Case for the page up key
    case GLUT_KEY_PAGE_UP:
        y += 0.1f; // Move camera up
        break;

        // Case for the page down key
    case GLUT_KEY_PAGE_DOWN:
        y -= 0.1f; // Move camera down
        break;
    }
}

// Function to process mouse button events
void mouseButton(int button, int state, int x, int y)
{
    // Check if the left mouse button is involved
    if (button == GLUT_LEFT_BUTTON)
    {
        // Check if the left mouse button is released
        if (state == GLUT_UP)
        {
            angle += deltaAngle; // Update the angle by adding the deltaAngle
            xOrigin = -1; // Reset the xOrigin to indicate no dragging in progress
        }
        else // The left mouse button is pressed down
        {
            xOrigin = x; // Set xOrigin to the current x position of the mouse
        }
    }
}

void mouseDrag(int x, int y)
{
    if (xOrigin >= 0)
    {
        float deltaY = (y - xOrigin) * 0.005f; // Calculate the change in y-coordinate
        ambientIntensity += deltaY; // Adjust ambient intensity based on mouse movement
        if (ambientIntensity < 0.2f) ambientIntensity = 0.2f; // Ensure ambientIntensity doesn't go below 0
        if (ambientIntensity > 0.7f) ambientIntensity = 0.7f; // Ensure ambientIntensity doesn't exceed 1
        glutPostRedisplay(); // Refresh the display after updating ambientIntensity
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // Initialize GLUT with command-line arguments
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // Set display mode with depth, double buffer, and RGBA color
    glutInitWindowPosition(100, 100); // Set initial window position
    glutInitWindowSize(800, 600); // Set initial window size
    glutCreateWindow("Penguin Animation with Camera"); // Create a window with the specified title

    glEnable(GL_DEPTH_TEST); // Enable depth testing to ensure objects render correctly based on depth
    glEnable(GL_LIGHTING); // Enable lighting in the scene
    glEnable(GL_COLOR_MATERIAL); // Enable coloring based on material properties

    // Load sky texture
    loadSkyTexture(); // Assuming this function loads and prepares the sky texture
    loadTexture(); // Assuming this function loads and prepares other textures

    // Play background music (assuming Windows platform)
    PlaySound(TEXT("C:\\zmisc\\bgm.wav"), NULL, SND_ASYNC | SND_LOOP); // Play background music asynchronously and loop it

    // Register callback functions
    glutDisplayFunc(renderScene); // Set the function to call for rendering the scene
    glutReshapeFunc(changeSize); // Set the function to call when the window size changes
    glutIdleFunc(renderScene); // Set the function to call when idle (continuously render scene)
    glutKeyboardFunc(processNormalKeys); // Set the function to handle normal keyboard key presses
    glutSpecialFunc(processSpecialKeys); // Set the function to handle special keys (e.g., arrow keys)
    glutMouseFunc(mouseButton); // Set the function to handle mouse button presses
    glutMotionFunc(mouseDrag); // Set the function to handle mouse drag movements

    // Set up a timer to call update function periodically
    glutTimerFunc(25, update, 0); // Call the update function after 25 milliseconds

    glutMainLoop(); // Enter the main loop of GLUT

    return 0; // Return 0 to indicate successful completion
}
