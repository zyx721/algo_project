#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <cmath>
#include <ctime>
#include <GL/freeglut.h>
#include <cstring>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const int DELAY_BEFORE_STORE_PAGE = 2000;  // 2000 milliseconds (2 seconds)
int storePageTransitionTimer = 0;

void drawTextBox(float x, float y, float width, float height) {
    glColor3f(0.0, 0.0, 0.0);  // Black
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y - height);
    glVertex2f(x, y - height);
    glEnd();
}

enum Page {
    HOME_PAGE,
    ENTER_NUMBER_PAGE,
    ENTER_NUMBER_OF_NODES_PAGE,
    NODE_INSERTION_PAGE,
    ENTER_NODE_VALUES_PAGE,
    STORE_VALUES_PAGE
};

char inputBuffer[10] = "";
int inputIndex = 0;
bool enteringValues = false;
Page currentPage = HOME_PAGE;

class Node {
public:
    Node *precedent;
    float val;
    Node *next;

    Node(float v) : precedent(NULL), val(v), next(NULL) {}
};

Node *head = NULL;
int n;

Node *createNode(float v) {
    Node *newNode = new Node(v);
    return newNode;
}

Node *add(Node *a, float v) {
    Node *b = createNode(v);
    if (a == NULL) {
        return b;
    }
    Node *c = a;
    while (c->next != NULL) {
        c = c->next;
    }
    b->precedent = c;
    c->next = b;
    return a;
}

float transitionSpeed = 0.005;;  // Adjust the transition speed as needed
GLfloat transitionOffset = 0.0;
// Add alpha value for fading effect
float alpha = 1.0;
// Constants for transition delay
const int DELAY_SECONDS = 3;  // Adjust the delay as needed
time_t startTime = 0;

// ...

void transitionToNextPage() {
    time_t currentTime = std::time(NULL);
    if (startTime == 0) {
        // Set the start time on the first call
        startTime = currentTime;
    }

    // Check if the elapsed time exceeds the delay
    if (currentTime - startTime >= DELAY_SECONDS) {
        transitionOffset = 0.0;
        int nextPage = static_cast<int>(currentPage) + 1;
        currentPage = static_cast<Page>(nextPage % (STORE_VALUES_PAGE + 1));  // Ensure it stays within bounds
        enteringValues = false;
        memset(inputBuffer, 0, sizeof(inputBuffer));
        inputIndex = 0;
        startTime = 0;  // Reset the start time
    }
}

// ...

// Timer callback function
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);  // Set the timer to call itself again in 1000 milliseconds (1 second)
}


void transitionToPreviousPage() {
    transitionOffset -= transitionSpeed;
    if (transitionOffset <= 0.0) {
        transitionOffset = 0.0;
        int prevPage = static_cast<int>(currentPage) - 1;
        if (prevPage < HOME_PAGE) {
            prevPage = STORE_VALUES_PAGE;  // Wrap around to the last page
        }
        currentPage = static_cast<Page>(prevPage);
    }
}



// Define a common background color for all pages
GLfloat startColor[3] = {0.7, 0.7, 1.0};  // Light blue
GLfloat endColor[3] = {0.0, 0.0, 0.7};      // Dark blue

// ... (other code)

void setCommonBackgroundColor() {
    glBegin(GL_QUADS);
    glColor3fv(startColor);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glColor3fv(endColor);
    glVertex2f(1.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();
}
void setCommonBackgroundColorWithTransition() {
    // Linear interpolation between startColor and endColor
    GLfloat currentColor[3];
    for (int i = 0; i < 3; ++i) {
        currentColor[i] = startColor[i] * (1 - transitionOffset) + endColor[i] * transitionOffset;
    }

    // Set alpha value based on transitionOffset
    GLfloat alphaValue = 1.0 - fabs(transitionOffset - 0.5) * 2.0; // Smooth fade-in and fade-out

    // Draw background with the calculated color and alpha
    glBegin(GL_QUADS);
    glColor4f(currentColor[0], currentColor[1], currentColor[2], alphaValue);
    glVertex2f(-1.0 + transitionOffset, 1.0);
    glVertex2f(1.0 + transitionOffset, 1.0);
    glColor4f(currentColor[0], currentColor[1], currentColor[2], alphaValue);
    glVertex2f(1.0 + transitionOffset, -1.0);
    glVertex2f(-1.0 + transitionOffset, -1.0);
    glEnd();
}

void drawArrow(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    const float arrowSize = 0.02;

    glBegin(GL_TRIANGLES);
    glVertex2f(x2, y2);
    glVertex2f(x2 - arrowSize, y2 - arrowSize / 2);
    glVertex2f(x2 - arrowSize, y2 + arrowSize / 2);
    glEnd();
}

void drawText(float x, float y, const char *format, ...) {
    va_list args;
    char buffer[256];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glColor3f(1.0, 1.0, 1.0); // White text for clarity
    glRasterPos2f(x, y);

    for (int i = 0; buffer[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buffer[i]);
    }
}
void drawText2(float x, float y, const char *format, ...) {
    va_list args;
    char buffer[256];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glColor3f(1.0, 1.0, 1.0); // White text for clarity
    glRasterPos2f(x, y);

    for (int i = 0; buffer[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_STROKE_ROMAN, buffer[i]);
    }
}
void drawSquare(float x, float y, float size) {
    glBegin(GL_QUADS);

    // Define gradient colors (center to edges)
    float centerColor[] = {0.5, 0.5, 0.5};  // Closer to edge color
    float edgeColor[] = {0.7, 0.7, 0.7};

    // Bottom-left vertex
    glColor3f(centerColor[0], centerColor[1], centerColor[2]);
    glVertex2f(x, y);

    // Bottom-right vertex
    glColor3f(edgeColor[0], edgeColor[1], edgeColor[2]);
    glVertex2f(x + size, y);

    // Top-right vertex
    glColor3f(edgeColor[0], edgeColor[1], edgeColor[2]);
    glVertex2f(x + size, y + size);

    // Top-left vertex
    glColor3f(centerColor[0], centerColor[1], centerColor[2]);
    glVertex2f(x, y + size);

    glEnd();
}



drawSquare3(float x, float y, float size) {
    glColor3f(1.0, 1.0, 1.0); // White lines for square
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}



void printListWithLines(float yOffset) {
    float x = -0.8;
    float y = yOffset;
    int i = 0;
    Node *current = head;
    int nodeCount = 0; // Variable to keep track of the number of nodes

    while (current != NULL) {
        // Draw node square
        drawSquare(x - 0.15, y - 0.15, 0.3);
         drawSquare3(x - 0.15, y + 0.15, 0.3);
        drawSquare3(x - 0.15, y - 0.45, 0.3);
        // Draw node value inside the square
        drawText(x, y + 0.05, "%.1f", current->val);
        drawText(x, y - 0.6, "%d", i);

        // Draw arrows for next
        if (current->next != NULL) {
            // Calculate the end coordinates for arrows
            float arrowX = x + 0.5;
            float arrowY = y + 0.3;
            glColor3f(0.0, 1.0, 0.0); // Green for arrows to next

            // Draw the line
            glBegin(GL_LINES);
            glVertex2f(x + 0.1, arrowY);
            glVertex2f(arrowX, arrowY);
            glEnd();

            // Draw arrowhead
            glBegin(GL_TRIANGLES);
            glVertex2f(arrowX, arrowY);
            glVertex2f(arrowX - 0.02, arrowY + 0.02);
            glVertex2f(arrowX - 0.02, arrowY - 0.02);
            glEnd();

            drawText(x, y + 0.3, "%d", i+1);
        } else {
            // If it's the last node, draw "NULL" in the next square
            drawText(x, y + 0.3, "NULL");
        }

        // Draw arrows for precedent (previous)
        if (current->precedent != NULL) {
            float arrowX = x - 0.5;
            float arrowY = y - 0.3;

            glColor3f(1.0, 0.0, 0.0); // Red for arrows to precedent
            glBegin(GL_LINES);
            glVertex2f(x - 0.1, arrowY);
            glVertex2f(arrowX, arrowY);
            glEnd();

            // Draw arrowhead to the left
            glBegin(GL_TRIANGLES);
            glVertex2f(arrowX, arrowY);
            glVertex2f(arrowX + 0.02, arrowY + 0.02);
            glVertex2f(arrowX + 0.02, arrowY - 0.02);
            glEnd();

                    if (current->precedent != NULL) {
                    drawText(x, y - 0.3, "%d", i-1);
                } else {
                    drawText(x, y - 0.3, "NULL");
                }
            } else {
                // If it's the first node, draw "NULL" in the precedent square
                drawText(x, y - 0.3, "NULL");
            }

        // Draw two squares above the main square


        // Move to the next node's position
        x += 0.6;
        current = current->next;
        nodeCount++;
        i++;
    }


    // If there are no nodes, draw "NULL" in the first and last squares
    if (nodeCount == 0) {
        drawText(-0.8, yOffset + 0.3, "NULL");
        drawText(-0.8, yOffset - 0.3, "NULL");
    }
}


// ... (other code)



void displayHomePage() {
    glClear(GL_COLOR_BUFFER_BIT);

    setCommonBackgroundColor();

    glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.15, 0.0);

    const char* buttonText = "Enter";
    for (int i = 0; buttonText[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, buttonText[i]);
    }

    glutSwapBuffers();
}

void displayEnterNumberOfNodesPage() {
    glClear(GL_COLOR_BUFFER_BIT);

    setCommonBackgroundColor();

    glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.45, 0.25);


    glColor3f(0.0, 0.0, 0.0);  // Black
    glBegin(GL_QUADS);
    glVertex2f(-0.25, 0.1);
    glVertex2f(0.25, 0.1);
    glVertex2f(0.25, -0.1);
    glVertex2f(-0.25, -0.1);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.1, 0.0);
    for (int i = 0; inputBuffer[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, inputBuffer[i]);
    }

    glutSwapBuffers();
}
void displayNodeInsertionPage() {

    glClear(GL_COLOR_BUFFER_BIT);

    setCommonBackgroundColor();
 // Draw the message indicating that this is the list after storing
    glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.5, 0.9);
    const char* storeMessage = "This is the list before storing:";
    for (int i = 0; storeMessage[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, storeMessage[i]);
    }
     glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.5, -0.5);
    const char* storeMessage2 = "click n to store the linked list";
    for (int i = 0; storeMessage2[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, storeMessage2[i]);
    }
    printListWithLines(0.4);

    if (enteringValues) {
        drawTextBox(-0.25, -0.7, 0.5, 0.2);


        glColor3f(1.0, 1.0, 1.0);  // White
        glRasterPos2f(-0.1, -0.7);
        for (int i = 0; i < inputIndex; ++i) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, inputBuffer[i]);
        }
    }



    glutSwapBuffers();
}

// Function to insert a node into the list in ascending order
Node* insertInAscendingOrder(Node* head, float value) {
    Node* newNode = createNode(value);

    if (head == NULL || value <= head->val) {
        newNode->next = head;
        if (head != NULL) {
            head->precedent = newNode;
        }
        return newNode;
    }

    Node* current = head;

    while (current->next != NULL && value > current->next->val) {
        current = current->next;
    }

    newNode->next = current->next;
    if (current->next != NULL) {
        current->next->precedent = newNode;
    }
    newNode->precedent = current;
    current->next = newNode;

    return head;
}

// Function to store the list in ascending order
Node* storeListAscending(Node* head) {
    Node* sortedList = NULL;
    Node* current = head;

    while (current != NULL) {
        sortedList = insertInAscendingOrder(sortedList, current->val);
        current = current->next;
    }

    return sortedList;
}

// Update the displayStoreValuesPage function to use the stored list
void printListWithLines2(float yOffset, Node* list) {
    float x = -0.8;
    float y = yOffset;
    int i = 0;
    Node* current = list;  // Use the provided list instead of the original 'head'
    int nodeCount = 0; // Variable to keep track of the number of nodes

    while (current != NULL) {
        // Draw node square
        drawSquare(x - 0.15, y - 0.15, 0.3);
        drawSquare3(x - 0.15, y + 0.15, 0.3);
        drawSquare3(x - 0.15, y - 0.45, 0.3);
        // Draw node value inside the square
        drawText(x, y + 0.05, "%.1f", current->val);
        drawText(x, y - 0.6, "%d", i);

        // Draw arrows for next
        if (current->next != NULL) {
            // Calculate the end coordinates for arrows
            float arrowX = x + 0.5;
            float arrowY = y + 0.3;
            glColor3f(0.0, 1.0, 0.0); // Green for arrows to next

            // Draw the line
            glBegin(GL_LINES);
            glVertex2f(x + 0.1, arrowY);
            glVertex2f(arrowX, arrowY);
            glEnd();

            // Draw arrowhead
            glBegin(GL_TRIANGLES);
            glVertex2f(arrowX, arrowY);
            glVertex2f(arrowX - 0.02, arrowY + 0.02);
            glVertex2f(arrowX - 0.02, arrowY - 0.02);
            glEnd();

            drawText(x, y + 0.3, "%d", i + 1);
        } else {
            // If it's the last node, draw "NULL" in the next square
            drawText(x, y + 0.3, "NULL");
        }

        // Draw arrows for precedent (previous)
        if (current->precedent != NULL) {
            float arrowX = x - 0.5;
            float arrowY = y - 0.3;

            glColor3f(1.0, 0.0, 0.0); // Red for arrows to precedent
            glBegin(GL_LINES);
            glVertex2f(x - 0.1, arrowY);
            glVertex2f(arrowX, arrowY);
            glEnd();

            // Draw arrowhead to the left
            glBegin(GL_TRIANGLES);
            glVertex2f(arrowX, arrowY);
            glVertex2f(arrowX + 0.02, arrowY + 0.02);
            glVertex2f(arrowX + 0.02, arrowY - 0.02);
            glEnd();

            if (current->precedent != NULL) {
                drawText(x, y - 0.3, "%d", i - 1);
            } else {
                drawText(x, y - 0.3, "NULL");
            }
        } else {
            // If it's the first node, draw "NULL" in the precedent square
            drawText(x, y - 0.3, "NULL");
        }

        // Move to the next node's position
        x += 0.6;
        current = current->next;
        nodeCount++;
        i++;
    }

    // If there are no nodes, draw "NULL" in the first and last squares
    if (nodeCount == 0) {
        drawText(-0.8, yOffset + 0.3, "NULL");
        drawText(-0.8, yOffset - 0.3, "NULL");
    }
}

void displayStoreValuesPage() {
    glClear(GL_COLOR_BUFFER_BIT);

    setCommonBackgroundColor();

    // Draw the message indicating that this is the list after storing
    glColor3f(1.0, 1.0, 1.0);  // White
    glRasterPos2f(-0.3, 0.8);
    const char* storeMessage = "This is the list after storing:";
    for (int i = 0; storeMessage[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, storeMessage[i]);
    }

    // Draw a separator line
    glColor3f(1.0, 1.0, 1.0);  // White
    glBegin(GL_LINES);
    glVertex2f(-1.0, 0.75);
    glVertex2f(1.0, 0.75);
    glEnd();

    // Store the list in ascending order
    Node* sortedList = storeListAscending(head);

    // Draw the stored list
    printListWithLines2(-0.4, sortedList);

    glutSwapBuffers();
}
// Update the main display function to handle all pages
void setBackgroundColorWithAlpha(float alpha) {
    glBegin(GL_QUADS);
    glColor4f(startColor[0], startColor[1], startColor[2], alpha);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glColor4f(endColor[0], endColor[1], endColor[2], alpha);
    glVertex2f(1.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();
}

void display() {
    setCommonBackgroundColorWithTransition();

    if (currentPage == HOME_PAGE) {
        displayHomePage();
    } else if (currentPage == ENTER_NUMBER_OF_NODES_PAGE) {
        displayEnterNumberOfNodesPage();
    } else if (currentPage == ENTER_NODE_VALUES_PAGE || currentPage == NODE_INSERTION_PAGE) {
        displayNodeInsertionPage();
    } else if (currentPage == STORE_VALUES_PAGE) {
        displayStoreValuesPage();
    }
}



int windowHeight = 600;  // Adjust the initial window height as needed

void mouse(int button, int state, int x, int y) {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (currentPage == HOME_PAGE && x >= windowWidth / 2 - 100 && x <= windowWidth / 2 + 100 &&
            y >= windowHeight / 2 - 50 && y <= windowHeight / 2 + 50) {
            currentPage = ENTER_NUMBER_OF_NODES_PAGE;
            glutPostRedisplay();
        } else if (currentPage == ENTER_NUMBER_OF_NODES_PAGE || currentPage == ENTER_NODE_VALUES_PAGE) {
            enteringValues = !enteringValues;

            if (enteringValues) {
                memset(inputBuffer, 0, sizeof(inputBuffer));
                inputIndex = 0;
            } else {
                if (currentPage == ENTER_NUMBER_OF_NODES_PAGE) {
                    n = atoi(inputBuffer);
                    head = NULL;

                    // Set the autoChangePage flag to true
                } else if (currentPage == ENTER_NODE_VALUES_PAGE) {
                    char* token = strtok(inputBuffer, " ");
                    while (token != NULL) {
                        float value = atof(token);
                        head = add(head, value);
                        token = strtok(NULL, " ");
                    }
                }

                glutPostRedisplay();
            }
        } else if (currentPage == NODE_INSERTION_PAGE && x >= windowWidth / 2 - 100 && x <= windowWidth / 2 + 100 &&
                   y >= windowHeight / 2 - 25 && y <= windowHeight / 2 + 25) {
            currentPage = STORE_VALUES_PAGE;
            glutPostRedisplay();
        }
    }
}



int  scrollOffset = 0;  // Variable to keep track of scrolling offset
extern int n;
// Number of nodes
int nodeCount = 0;
int valuesInserted = 0;

void keyboard(unsigned char key, int x, int y) {
    static int inputCount = 0;

    if (key == 'q') {
        exit(0); // Exit on 'q' press
    }

    if (enteringValues && inputIndex < sizeof(inputBuffer) - 1) {
        if ((key >= '0' && key <= '9') || key == ' ') {
            // Numeric key or space key pressed, add it to the buffer
            inputBuffer[inputIndex++] = key;
        } else if (key == 13) {
            // Enter key pressed
            if (inputIndex > 0) {
                // Validate the entered number
                int value = atoi(inputBuffer);
                if (value >= 1 && value <= 10) {
                    // Process entered values based on the current page
                    if (currentPage == ENTER_NUMBER_OF_NODES_PAGE) {
                        // Get the number of nodes and create an empty linked list
                        n = value;
                        head = NULL;

                        // Switch to the next page
                        currentPage = ENTER_NODE_VALUES_PAGE;
                    } else if (currentPage == ENTER_NODE_VALUES_PAGE) {
                        // Split entered values and add them to the linked list
                        char* token = strtok(inputBuffer, " ");
                        while (token != NULL) {
                            float floatValue = atof(token);
                            head = add(head, floatValue);
                            token = strtok(NULL, " ");
                        }

                        // Increment the count of inserted values
                        valuesInserted += 1;

                        // Check if the user has inserted values for all nodes
                        if (valuesInserted >= n) {
                            // Set the timer for the delay before transitioning to the Store Values Page
                            storePageTransitionTimer = glutGet(GLUT_ELAPSED_TIME) + DELAY_BEFORE_STORE_PAGE;
                        }
                    }

                    // Reset buffer and index
                    memset(inputBuffer, 0, sizeof(inputBuffer));
                    inputIndex = 0;

                    glutPostRedisplay();  // Request a redraw to update the display
                }
            }
        } else if (key == 8 && inputIndex > 0) {
            // Backspace key pressed, delete the last character
            inputBuffer[--inputIndex] = '\0';
        }

        glutPostRedisplay();  // Request a redraw to update the display
    }

    // Check if the delay timer has expired
    if (key == 'n') {
        // Transition to the Store Values Page immediately
        currentPage = STORE_VALUES_PAGE;
        glutPostRedisplay();
    }

    // Check if the delay timer has expired
    if (storePageTransitionTimer > 0 && glutGet(GLUT_ELAPSED_TIME) >= storePageTransitionTimer) {
        // Transition to the Store Values Page
        currentPage = STORE_VALUES_PAGE;
        glutPostRedisplay();
        storePageTransitionTimer = 0;  // Reset the timer
    }
}


int main(int argc, char** argv) {
    int n;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int windowWidth = 800;
    int windowHeight = 600;
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);

    glutCreateWindow("Three-Page Application");
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);

    glClearColor(0.0, 0.0, 0.4, 1.0);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glutReshapeWindow(windowWidth, windowHeight);
    glutMainLoop();
    return 0;
}
