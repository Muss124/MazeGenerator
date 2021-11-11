#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <stack>

void display();
void reshape(int w, int h);
void init();
void timer(int a);

class Node
{
public:
	bool sides[4];
	bool visited = false;
	bool isInStack = false;
	bool isMarked = false;
	int x, y;
	Node(int x, int y, bool side1, bool side2, bool side3, bool side4)
	{
		this->x = x;
		this->y = y;
		sides[0] = side1;
		sides[1] = side2;
		sides[2] = side3;
		sides[3] = side4;
	}
	Node* checkNeighbours();
private:
	int transformIndex(int x, int y);
	static void RemoveWalls(Node* cur, Node* next);
};

const int xAmount = 30, yAmount = 20;

const int dif = 25;

const int framerate = 60;
const int windowWidth = xAmount * dif;
const int windowHeight = yAmount * dif;

std::vector<Node*> cells;
Node* current;
std::stack<Node*> nodesStack;

int main(int argc, char** argv)
{
	std::srand(std::time(NULL));
	for (size_t i = 0; i < xAmount; i++)
	{
		for (size_t j = 0; j < yAmount; j++)
		{
			cells.push_back(new Node(dif * i, dif * j, true, true, true, true));
		}
	}
	current = cells[0];
	current->visited = true;

	

	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	glutInitWindowPosition(200, 100);
	glutInitWindowSize(windowWidth, windowHeight);

	glutCreateWindow("Test");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);
	init();

	glutMainLoop();

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	for (size_t i = 0; i < cells.size(); i++)
	{
		if (cells[i]->visited)
		{
			//std::cout << i << std::endl;
			glBegin(GL_QUADS);
			glColor3f(152.0/255, 251.0/255, 152.0/255.0);
			
			//highlight current cell
			if (cells[i] == current)
			{
				glColor3f(1, 1, 0);
			}
			
			//highlight cells that are in stack currently
			if (cells[i]->isInStack)
			{
				glColor3f(209.0/255, 237.0/255, 242.0/255);
			}

			//highlight enter and exit 
			if (cells[i]->isMarked)
			{
				glColor3f(255.0 / 255, 127.0 / 255, 127.0 / 255);
			}


			glVertex2i(cells[i]->x, cells[i]->y);
			glVertex2i(cells[i]->x, cells[i]->y + dif);
			glVertex2i(cells[i]->x + dif, cells[i]->y + dif);
			glVertex2i(cells[i]->x + dif, cells[i]->y);
			glColor3f(1, 1, 1);
			glEnd();
		}

		glBegin(GL_LINES);
		if (cells[i]->sides[0])
		{
			glVertex2i(cells[i]->x, cells[i]->y);
			glVertex2i(cells[i]->x, cells[i]->y + dif);
		}
		if (cells[i]->sides[1])
		{
			glVertex2i(cells[i]->x, cells[i]->y + dif);
			glVertex2i(cells[i]->x + dif, cells[i]->y + dif);
		}
		if (cells[i]->sides[2])
		{
			glVertex2i(cells[i]->x + dif, cells[i]->y + dif);
			glVertex2i(cells[i]->x + dif, cells[i]->y);
		}
		if (cells[i]->sides[3])
		{
			glVertex2i(cells[i]->x + dif, cells[i]->y);
			glVertex2i(cells[i]->x, cells[i]->y);
		}
		glEnd();
	}	
	
	

	glutSwapBuffers();
}
void init()
{
	glClearColor(0, 0, 0, 1);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1 * dif, dif * (xAmount + 1), -1 * dif, dif * (yAmount + 1));
	glMatrixMode(GL_MODELVIEW);
}

void timer(int a)
{	
	glutPostRedisplay();	
	if (current != NULL)
	{
		current = current->checkNeighbours();
		if (current != NULL)
		{
			current->visited = true;
		}
	}
	else
	{
		cells[0]->isMarked = true;
		cells[cells.size() - 1]->isMarked = true;
	}
	glutTimerFunc(1000/framerate, timer, 0);
}

Node* Node::checkNeighbours()
{
	std::vector<Node*> neighbours;

	if (x - dif >= 0)
	{
		if (!cells[transformIndex(x - dif, y)]->visited)
		{
			neighbours.push_back(cells[transformIndex(x - dif, y)]);
		}
	}
	if (y - dif >= 0)
	{
		if (!cells[transformIndex(x, y - dif)]->visited)
		{
			neighbours.push_back(cells[transformIndex(x, y - dif)]);
		}
	}
	if (x + dif < xAmount * dif)
	{
		if (!cells[transformIndex(x + dif, y)]->visited)
		{
			neighbours.push_back(cells[transformIndex(x + dif, y)]);
		}
	}
	if (y + dif < yAmount * dif)
	{
		if (!cells[transformIndex(x, y + dif)]->visited)
		{
			neighbours.push_back(cells[transformIndex(x, y + dif)]);
		}
	}
	
	if (neighbours.size() != 0)
	{
		int index = std::rand() % neighbours.size();
		neighbours[index]->isInStack = true;
		nodesStack.push(neighbours[index]);
		Node::RemoveWalls(this, neighbours[index]);
		return neighbours[index];
	}
	else
	{
		if (nodesStack.size() != 0)
		{
			Node* next = nodesStack.top();
			next->isInStack = false;
			nodesStack.pop();
			return next;
		}
		else
		{
			return NULL;
		}
		
	}
}

int Node::transformIndex(int x, int y)
{
	return x / dif *yAmount  + y / dif;
}

void Node::RemoveWalls(Node* cur, Node* next)
{
	if (cur->x == next->x)
	{
		if (cur->y < next->y)
		{
			next->sides[3] = false;
			cur->sides[1] = false;
		}
		else
		{
			next->sides[1] = false;
			cur->sides[3] = false;
		}
	}
	else
	{
		if (cur->x < next->x)
		{
			next->sides[0] = false;
			cur->sides[2] = false;
		}
		else
		{
			next->sides[2] = false;
			cur->sides[0] = false;
		}
	}
}