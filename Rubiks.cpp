#include <GL/gl.h>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "Vector.h"
#include "GfxOpenGL.h"
#include "Rubiks.h"

extern GfxOpenGL* g_glRender;

// Platform-dependent GUI dialog display functions.
void RubiksSaveErrorDlg();
void RubiksLoadErrorDlg();

RubiksCube::RubiksCube()
{
	rubiksReady = false;
}

RubiksCube::~RubiksCube()
{
	//Save the Rubik's Cube to a file
	FILE* fp = fopen("Rubcube.dat", "wb");
	if (fp != NULL)
	{
		fwrite(faceRotation, sizeof(faceRotation), 1, fp);
		fwrite(corners, sizeof(corners), 1, fp);
		fwrite(cornerRot, sizeof(cornerRot), 1, fp);
		fwrite(edges, sizeof(edges), 1, fp);
		fwrite(edgeRot, sizeof(edgeRot), 1, fp);
		fclose(fp);
	}
	else
		RubiksSaveErrorDlg();

	//Change GL state back
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void RubiksCube::NewRubiksCube()
{
	memset(faceRotation, 0, sizeof(faceRotation));
	for (unsigned i = 0; i < 8; i++)
		corners[i] = (cornerID)i;
	memset(cornerRot, 0, sizeof(cornerRot));
	for (unsigned i = 0; i < 12; i++)
		edges[i] = (edgeID)i;
	memset(edgeRot, 0, sizeof(edgeRot));
}

void RubiksCube::Init()
{
	//Initialize some variables to prevent crashing
	currentFace = WHITE;
	//We'll store our cube in a file
	FILE* fp = fopen("Rubcube.dat", "rb");
	if (fp == NULL)
	{
		//Create a NEW cube
		NewRubiksCube();
		rubiksReady = true;
	}
	else
	{
		fread(faceRotation, sizeof(faceRotation), 1, fp);
		fread(corners, sizeof(corners), 1, fp);
		fread(cornerRot, sizeof(cornerRot), 1, fp);
		fread(edges, sizeof(edges), 1, fp);
		fread(edgeRot, sizeof(edgeRot), 1, fp);
		fclose(fp);

		//Check our readings or correct then if they failed
#define A_OK 0
#define FACES_FAIL 2
#define CORNERS_FAIL 4
#define EDGES_FAIL 8
#define CORNERROT_FAIL 16
#define EDGEROT_FAIL 32
		unsigned correctCube = A_OK;

		for (unsigned i = 0; i < 6; i++)
		{
			if (faceRotation[i] <= -45.0f)
			{
				double intPart; //Temporary memory place holder
				faceRotation[i] /= 45.0f;
				faceRotation[i] = modf(faceRotation[i], &intPart) * 45.0f;
				correctCube |= FACES_FAIL;
			}
			if (faceRotation[i] > 45.0f)
			{
				double intPart; //Temporary memory place holder
				faceRotation[i] /= 45.0f;
				faceRotation[i] = modf(faceRotation[i], &intPart) * 45.0f;
				correctCube |= FACES_FAIL;
			}
		}

		bool checklist[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
		for (unsigned i = 0; i < 8; i++)
		{
			if (corners[i] > 7)
				correctCube |= CORNERS_FAIL;
			else
				checklist[corners[i]] = true;
		}
		if (correctCube & CORNERS_FAIL)
		{
			for (unsigned i = 0; i < 8; i++)
			{
				if (corners[i] > 7)
				{
					for (unsigned j = 0; j < 8; j++)
					{
						if (checklist[j] == false)
						{
							checklist[j] = true;
							corners[i] = (cornerID)j;
							break;
						}
					}
				}
			}
		}

		for (unsigned i = 0; i < 8; i++)
		{
			if (cornerRot[i] > 2)
			{
				cornerRot[i] = 2;
				correctCube |= CORNERROT_FAIL;
			}
		}

		memset(checklist, 0, sizeof(checklist));
		for (unsigned i = 0; i < 12; i++)
		{
			if (edges[i] > 11)
				correctCube |= EDGES_FAIL;
			else
				checklist[edges[i]] = true;
		}
		if (correctCube & EDGES_FAIL)
		{
			for (unsigned i = 0; i < 12; i++)
			{
				if (edges[i] > 11)
				{
					for (unsigned j = 0; j < 12; j++)
					{
						if (checklist[j] == false)
						{
							checklist[j] = true;
							edges[i] = (edgeID)j;
							break;
						}
					}
				}
			}
		}

		for (unsigned i = 0; i < 12; i++)
		{
			if (edgeRot[i] > 1)
			{
				edgeRot[i] = 1;
				correctCube |= EDGEROT_FAIL;
			}
		}

		rubiksReady = true;
		if (correctCube != A_OK)
			RubiksLoadErrorDlg();
#undef A_OK
#undef FACES_FAIL
#undef CORNERS_FAIL
#undef EDGES_FAIL
#undef CORNERROT_FAIL
#undef EDGEROT_FAIL
	}

	//Initalize GL state settings
	glShadeModel(GL_FLAT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

void RubiksCube::Prepare(float dt)
{
}

void RubiksCube::Render()
{
	if (!rubiksReady)
		return;
	//Draw the pitch black center
	glDisable(GL_LIGHTING);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	/*GLfloat bCenterColors[8][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
								{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };*/
	GLfloat bCenterVerts[8][3] = { {1.2f, 1.2f, 1.2f}, {-1.2f, 1.2f, 1.2f}, {-1.2f, -1.2f, 1.2f}, {1.2f, -1.2f, 1.2f},
								{1.2f, 1.2f, -1.2f}, {-1.2f, 1.2f, -1.2f}, {-1.2f, -1.2f, -1.2f}, {1.2f, -1.2f, -1.2f} };
	GLubyte bCenterIndices[] = { 0, 1, 2, 3, 4, 5, 1, 0, 3, 2, 6, 7, 4, 0, 3, 7, 1, 5, 6, 2, 7, 6, 5, 4 };
	//glColorPointer(3, GL_FLOAT, 0, bCenterColors);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, 0, bCenterVerts);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, bCenterIndices);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_LIGHTING);
	//Parse the Rubik's Cube

	//Each Rubik's Cube piece is rendered from a single block mesh.  The block mesh is basically a cube
	//with the corners and edges cut off.  The block mesh is flat shaded and drawn with a single call to
	//glDrawElements().  However, it is not possible to pair each face of the block mesh with a single
	//vertex that has a vertex normal associated with it for flat shading, so there is one corner face
	//that is not drawn.  The block mesh is rotated so that only faces opposite of that missing face
	//are visible and have their colors set.
	GLfloat blockVerts[30][3] = { {0.9f, 0.9f, 1.0f}, {-0.9f, 0.9f, 1.0f}, {-0.9f, -0.9f, 1.0f}, {0.9f, -0.9f, 1.0f},
							{-0.9f, 0.9f, -1.0f}, {0.9f, 0.9f, -1.0f}, {0.9f, -0.9f, -1.0f}, {-0.9f, -0.9f, -1.0f},
							{-0.9f, 1.0f, 0.9f}, {0.9f, 1.0f, 0.9f}, {0.9f, 1.0f, -0.9f}, {-0.9f, 1.0f, -0.9f},
							{0.9f, -1.0f, 0.9f}, {-0.9f, -1.0f, 0.9f}, {-0.9f, -1.0f, -0.9f}, {0.9f, -1.0f, -0.9f},
							{1.0f, 0.9f, -0.9f}, {1.0f, 0.9f, 0.9f}, {1.0f, -0.9f, 0.9f}, {1.0f, -0.9f, -0.9f},
							{-1.0f, 0.9f, 0.9f}, {-1.0f, 0.9f, -0.9f}, {-1.0f, -0.9f, -0.9f}, {-1.0f, -0.9f, 0.9f},
	{-0.9f, -0.9f, 1.0f}, {-1.0f, -0.9f, 0.9f}, {-0.9f, -1.0f, 0.9f}, {0.9f, -0.9f, -1.0f}, {1.0f, -0.9f, -0.9f}, {0.9f, -1.0f, -0.9f} }; //One "subcube"
	GLfloat blockColors[30][3] = { {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f},
	{0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f} };
	GLfloat blockNormals[30][3] = { {0.0f, 0.0f, 1.0f}, {-0.5773503f, 0.5773503f, 0.5773503f}, {0.0f, -0.7071068f, 0.7071068f}, {0.7071068f, 0.0f, 0.7071068f},
	{0.0f, 0.7071068f, -0.7071068f}, {0.5773503f, 0.5773503f, -0.5773503f}, {0.0f, 0.0f, -1.0f}, {-0.5773503f, -0.5773503f, -0.5773503f},
	{-0.7071068f, 0.7071068f, 0.0f}, {0.0f, 0.7071068f, 0.7071068f}, {0.7071068f, 0.7071068f, 0.0f}, {0.0f, 1.0f, 0.0f},
	{0.5773503f, -0.5773503f, 0.5773503f}, {0.0f, -1.0f, 0.0f}, {-0.7071068f, -0.7071068f, 0.0f}, {0.0f, -0.7071068f, -0.7071068f},
	{0.7071068f, 0.0f, -0.7071068f}, {0.5773503f, 0.5773503f, 0.5773503f}, {1.0f, 0.0f, 0.0f}, {0.7071068f, -0.7071068f, 0.0f},
	{-1.0f, 0.0f, 0.0f}, {-0.5773503f, 0.5773503f, -0.5773503f}, {-0.7071068f, 0.0f, -0.7071068f}, {-0.7071068f, 0.0f, 0.7071068f},
	{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {-0.5773503f, -0.5773503f, 0.5773503f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5773503f, -0.5773503f, -0.5773503f} };
	GLubyte blockIndices[] = { 1, 2, 0, 2, 3, 0, 1, 0, 9, 8, 1, 9, 8, 9, 11, 9, 10, 11, 17, 16, 10, 9, 17, 10,
		16, 17, 18, 19, 16, 18, 17, 0, 3, 18, 17, 3, 9, 0, 17, 4, 5, 6, 7, 4, 6, 10, 5, 4,
		11, 10, 4, 6, 5, 16, 19, 6, 16, 10, 16, 5, 21, 22, 20, 22, 23, 20, 4, 7, 22, 21, 4, 22,
		11, 21, 8, 21, 20, 8, 11, 4, 21, 2, 1, 23, 1, 20, 23, 8, 20, 1, 15, 12, 13, 14, 15, 13,
		12, 3, 2, 13, 12, 2, 18, 3, 12, 18, 12, 19, 12, 15, 19, 7, 6, 15, 14, 7, 15, 23, 22, 14,
		13, 23, 14, 14, 22, 7,
		/*2, 23, 13, 6, 19, 15*/
		24, 25, 26, 27, 28, 29 };
	glVertexPointer(3, GL_FLOAT, 0, blockVerts);
	glColorPointer(3, GL_FLOAT, 0, blockColors);
	glNormalPointer(GL_FLOAT, 0, blockNormals);

#define SET_WHITE(p) blockColors[p][0] = 1.0f; blockColors[p][1] = 1.0f; blockColors[p][2] = 1.0f;
#define SET_YELLOW(p) blockColors[p][0] = 1.0f; blockColors[p][1] = 1.0f; blockColors[p][2] = 0.0f;
#define SET_GREEN(p) blockColors[p][0] = 0.0f; blockColors[p][1] = 0.625f; blockColors[p][2] = 0.0f;
#define SET_BLUE(p) blockColors[p][0] = 0.0f; blockColors[p][1] = 0.0f; blockColors[p][2] = 1.0f;
#define SET_RED(p) blockColors[p][0] = 1.0f; blockColors[p][1] = 0.0f; blockColors[p][2] = 0.0f;
#define SET_ORANGE(p) blockColors[p][0] = 1.0f; blockColors[p][1] = 0.5f; blockColors[p][2] = 0.0f;
#define SET_BLACK(p) blockColors[p][0] = 0.2f; blockColors[p][1] = 0.2f; blockColors[p][2] = 0.2f;
	//Draw the faces
	GLfloat faceTrans[6][3] = { {0.0f, 0.0f, 2.0f}, {0.0f, 0.0f, -2.0f}, {2.0f, 0.0f, 0.0f},
								{-2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f}, {0.0f, -2.0f, 0.0f} };
	GLfloat faceRotAxes[6][4] = { {0.0f, 0.0f, 0.0f, 0.0f}, {180.0f, 0.0f, 1.0f, 0.0f}, {90.0f, 0.0f, 1.0f, 0.0f},
								{-90.0f, 0.0f, 1.0f, 0.0f}, {-90.0f, 1.0f, 0.0f, 0.0f}, {90.0f, 1.0f, 0.0f, 0.0f} };
	for (unsigned i = 0; i < 6; i++)
	{
		glPushMatrix();
		SetFaceColors(blockColors, i);
		glTranslatef(faceTrans[i][0], faceTrans[i][1], faceTrans[i][2]);
		glRotatef(faceRotAxes[i][0], faceRotAxes[i][1], faceRotAxes[i][2], faceRotAxes[i][3]);
		glRotatef(faceRotation[i], 0.0f, 0.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, 132, GL_UNSIGNED_BYTE, blockIndices);
		glPopMatrix();
	}

	//Rule for drawing 45 degree rotated faces:
	//To properly find the other associated parts, look at the face and
	//go counterclockwise, then bring the parts from those cells back
	//to the rotated position.

	//Draw the corners
	//This array stores which faces a corner is shared by
	unsigned cnrFaceShares[8][3] = { {0, 4, 2}, {0, 4, 3}, {0, 5, 3}, {0, 5, 2},
									{1, 4, 3}, {1, 4, 2}, {1, 5, 2}, {1, 5, 3} };
	//This array stores the axes which a face or edge piece should be rotated around
	//according to a given face. (connected piece axis)
	GLfloat cnctPcAx[6][3] = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f},
									{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} };
	GLfloat cnrTrans[8][3] = { {2.0f, 2.0f, 2.0f}, {-2.0f, 2.0f, 2.0f}, {-2.0f, -2.0f, 2.0f}, {2.0f, -2.0f, 2.0f},
								{2.0f, 2.0f, 2.0f}, {-2.0f, 2.0f, 2.0f}, {-2.0f, -2.0f, 2.0f}, {2.0f, -2.0f, 2.0f} };
	GLfloat curRotSpot = 0.0f; //Specifies where to rotate to draw the current corner on either the front or back face
	for (unsigned i = 0; i < 8; i++)
	{
		glPushMatrix();
		SetCornerColors(blockColors, i);
		RotateCorner(blockColors, cornerRot[corners[i]]);
		if (faceRotation[cnrFaceShares[i][0]] != 0.0f)
			glRotatef(faceRotation[cnrFaceShares[i][0]], cnctPcAx[cnrFaceShares[i][0]][0], cnctPcAx[cnrFaceShares[i][0]][1], cnctPcAx[cnrFaceShares[i][0]][2]);
		else if (faceRotation[cnrFaceShares[i][1]] != 0.0f)
			glRotatef(faceRotation[cnrFaceShares[i][1]], cnctPcAx[cnrFaceShares[i][1]][0], cnctPcAx[cnrFaceShares[i][1]][1], cnctPcAx[cnrFaceShares[i][1]][2]);
		else
			glRotatef(faceRotation[cnrFaceShares[i][2]], cnctPcAx[cnrFaceShares[i][2]][0], cnctPcAx[cnrFaceShares[i][2]][1], cnctPcAx[cnrFaceShares[i][2]][2]);
		if (i >= 4) //Draw the back corners
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(cnrTrans[i][0], cnrTrans[i][1], cnrTrans[i][2]);
		glRotatef(curRotSpot, 0.0f, 0.0f, 1.0f);
		curRotSpot += 90.0f;
		glDrawElements(GL_TRIANGLES, 132, GL_UNSIGNED_BYTE, blockIndices);
		glPopMatrix();
	}

	//Draw the edges
	unsigned edgFaceShares[12][2] = { {0, 5}, {0, 2}, {0, 4}, {2, 5}, {2, 1}, {2, 4},
									{1, 5}, {1, 3}, {1, 4}, {3, 5}, {3, 0}, {3, 4} };
	curRotSpot = -180.0f; //Specifies where to rotate to draw the current edge on the current face
	GLfloat curFaceRot = 0.0f; //Specifies how to rotate to get to the current face
	SET_BLACK(18);
	for (unsigned i = 0; i < 12; i++)
	{
		glPushMatrix();
		SetEdgeColors(blockColors, i);
		RotateEdge(blockColors, edgeRot[edges[i]]);
		if (faceRotation[edgFaceShares[i][0]] != 0.0f)
			glRotatef(faceRotation[edgFaceShares[i][0]], cnctPcAx[edgFaceShares[i][0]][0], cnctPcAx[edgFaceShares[i][0]][1], cnctPcAx[edgFaceShares[i][0]][2]);
		else
			glRotatef(faceRotation[edgFaceShares[i][1]], cnctPcAx[edgFaceShares[i][1]][0], cnctPcAx[edgFaceShares[i][1]][1], cnctPcAx[edgFaceShares[i][1]][2]);
		if (i != 0 && i % 3 == 0)
			curFaceRot += 90.0f;
		glRotatef(curFaceRot, 0.0f, 1.0f, 0.0f);
		glRotatef(curRotSpot, 0.0f, 0.0f, 1.0f);
		if (curRotSpot == 0.0f)
			curRotSpot = -180.0f;
		else
			curRotSpot += 90.0f;
		glTranslatef(0.0f, 2.0f, 2.0f);
		glDrawElements(GL_TRIANGLES, 132, GL_UNSIGNED_BYTE, blockIndices);
		glPopMatrix();
	}
}

void RubiksCube::FindMousePick(int x, int y)
{
	g_glRender->Get3DPick(x, y, currentFace, oldPick);
	//Align proper faces before allowing rotation
	switch (currentFace)
	{
	case WHITE:
	case YELLOW:
		faceRotation[GREEN] = 0.0f;
		faceRotation[BLUE] = 0.0f;
		faceRotation[RED] = 0.0f;
		faceRotation[ORANGE] = 0.0f;
		break;
	case GREEN:
	case BLUE:
		faceRotation[WHITE] = 0.0f;
		faceRotation[YELLOW] = 0.0f;
		faceRotation[RED] = 0.0f;
		faceRotation[ORANGE] = 0.0f;
		break;
	case RED:
	case ORANGE:
		faceRotation[WHITE] = 0.0f;
		faceRotation[YELLOW] = 0.0f;
		faceRotation[GREEN] = 0.0f;
		faceRotation[BLUE] = 0.0f;
		break;
	}
}

void RubiksCube::RotateFace(int x, int y)
{
	Point newPick;
	g_glRender->Get3DDeltaPick(x, y, currentFace, newPick);
	float ang1, ang2;
	switch (currentFace)
	{
	case WHITE:
		ang1 = atan2(newPick.y, newPick.x), ang2 = atan2(oldPick.y, oldPick.x);
		break;
	case YELLOW:
		ang1 = atan2(oldPick.y, oldPick.x), ang2 = atan2(newPick.y, newPick.x);
		break;
	case GREEN:
		ang1 = atan2(oldPick.y, oldPick.z), ang2 = atan2(newPick.y, newPick.z);
		break;
	case BLUE:
		ang1 = atan2(newPick.y, newPick.z), ang2 = atan2(oldPick.y, oldPick.z);
		break;
	case RED:
		ang1 = atan2(oldPick.z, oldPick.x), ang2 = atan2(newPick.z, newPick.x);
		break;
	case ORANGE:
		ang1 = atan2(newPick.z, newPick.x), ang2 = atan2(oldPick.z, oldPick.x);
		break;
	}
	if (ang1 > 0.0f && ang2 < 0.0f || ang1 < 0.0f && ang2 > 0.0f)
		faceRotation[currentFace] += RAD2DEG(ang1 + ang2);
	else
		faceRotation[currentFace] += RAD2DEG(ang1 - ang2);
	if (faceRotation[currentFace] > 45.0f)
	{
		RotateFace90(true);
		faceRotation[currentFace] = -90.0f + faceRotation[currentFace];
	}
	else if (faceRotation[currentFace] <= -45.0f)
	{
		RotateFace90(false);
		faceRotation[currentFace] = 90.0f + faceRotation[currentFace];
	}
	oldPick = newPick;
}

void RubiksCube::RotateFace90(bool ccw)
{
	switch (currentFace)
	{
	case WHITE:
		if (ccw == true)
		{
			unsigned temp = corners[0];
			corners[0] = corners[3];
			corners[3] = corners[2];
			corners[2] = corners[1];
			corners[1] = (cornerID)temp;

			temp = edges[0];
			edges[0] = edges[10];
			if (edgeRot[edges[0]] == 0)
				edgeRot[edges[0]] = 1;
			else
				edgeRot[edges[0]] = 0;
			edges[10] = edges[2];
			if (edgeRot[edges[10]] == 0)
				edgeRot[edges[10]] = 1;
			else
				edgeRot[edges[10]] = 0;
			edges[2] = edges[1];
			edges[1] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[0];
			corners[0] = corners[1];
			corners[1] = corners[2];
			corners[2] = corners[3];
			corners[3] = (cornerID)temp;

			temp = edges[0];
			edges[0] = edges[1];
			edges[1] = edges[2];
			edges[2] = edges[10];
			if (edgeRot[edges[2]] == 0)
				edgeRot[edges[2]] = 1;
			else
				edgeRot[edges[2]] = 0;
			edges[10] = (edgeID)temp;
			if (edgeRot[edges[10]] == 0)
				edgeRot[edges[10]] = 1;
			else
				edgeRot[edges[10]] = 0;
		}
		break;
	case YELLOW:
		if (ccw == true)
		{
			unsigned temp = corners[4];
			corners[4] = corners[7];
			corners[7] = corners[6];
			corners[6] = corners[5];
			corners[5] = (cornerID)temp;

			temp = edges[6];
			edges[6] = edges[4];
			if (edgeRot[edges[6]] == 0)
				edgeRot[edges[6]] = 1;
			else
				edgeRot[edges[6]] = 0;
			edges[4] = edges[8];
			if (edgeRot[edges[4]] == 0)
				edgeRot[edges[4]] = 1;
			else
				edgeRot[edges[4]] = 0;
			edges[8] = edges[7];
			edges[7] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[4];
			corners[4] = corners[5];
			corners[5] = corners[6];
			corners[6] = corners[7];
			corners[7] = (cornerID)temp;

			temp = edges[6];
			edges[6] = edges[7];
			edges[7] = edges[8];
			edges[8] = edges[4];
			if (edgeRot[edges[8]] == 0)
				edgeRot[edges[8]] = 1;
			else
				edgeRot[edges[8]] = 0;
			edges[4] = (edgeID)temp;
			if (edgeRot[edges[4]] == 0)
				edgeRot[edges[4]] = 1;
			else
				edgeRot[edges[4]] = 0;
		}
		break;
	case GREEN:
		if (ccw == true)
		{
			unsigned temp = corners[5];
			corners[5] = corners[6];
			if (cornerRot[corners[5]] != 2)
				cornerRot[corners[5]]++;
			else
				cornerRot[corners[5]] = 0;
			corners[6] = corners[3];
			if (cornerRot[corners[6]] != 0)
				cornerRot[corners[6]]--;
			else
				cornerRot[corners[6]] = 2;
			corners[3] = corners[0];
			if (cornerRot[corners[3]] != 2)
				cornerRot[corners[3]]++;
			else
				cornerRot[corners[3]] = 0;
			corners[0] = (cornerID)temp;
			if (cornerRot[corners[0]] != 0)
				cornerRot[corners[0]]--;
			else
				cornerRot[corners[0]] = 2;

			temp = edges[3];
			edges[3] = edges[1];
			if (edgeRot[edges[3]] == 0)
				edgeRot[edges[3]] = 1;
			else
				edgeRot[edges[3]] = 0;
			edges[1] = edges[5];
			if (edgeRot[edges[1]] == 0)
				edgeRot[edges[1]] = 1;
			else
				edgeRot[edges[1]] = 0;
			edges[5] = edges[4];
			edges[4] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[5];
			corners[5] = corners[0];
			if (cornerRot[corners[5]] != 2)
				cornerRot[corners[5]]++;
			else
				cornerRot[corners[5]] = 0;
			corners[0] = corners[3];
			if (cornerRot[corners[0]] != 0)
				cornerRot[corners[0]]--;
			else
				cornerRot[corners[0]] = 2;
			corners[3] = corners[6];
			if (cornerRot[corners[3]] != 2)
				cornerRot[corners[3]]++;
			else
				cornerRot[corners[3]] = 0;
			corners[6] = (cornerID)temp;
			if (cornerRot[corners[6]] != 0)
				cornerRot[corners[6]]--;
			else
				cornerRot[corners[6]] = 2;

			temp = edges[3];
			edges[3] = edges[4];
			edges[4] = edges[5];
			edges[5] = edges[1];
			if (edgeRot[edges[5]] == 0)
				edgeRot[edges[5]] = 1;
			else
				edgeRot[edges[5]] = 0;
			edges[1] = (edgeID)temp;
			if (edgeRot[edges[1]] == 0)
				edgeRot[edges[1]] = 1;
			else
				edgeRot[edges[1]] = 0;
		}
		break;
	case BLUE:
		if (ccw == true)
		{
			unsigned temp = corners[1];
			corners[1] = corners[2];
			if (cornerRot[corners[1]] != 2)
				cornerRot[corners[1]]++;
			else
				cornerRot[corners[1]] = 0;
			corners[2] = corners[7];
			if (cornerRot[corners[2]] != 0)
				cornerRot[corners[2]]--;
			else
				cornerRot[corners[2]] = 2;
			corners[7] = corners[4];
			if (cornerRot[corners[7]] != 2)
				cornerRot[corners[7]]++;
			else
				cornerRot[corners[7]] = 0;
			corners[4] = (cornerID)temp;
			if (cornerRot[corners[4]] != 0)
				cornerRot[corners[4]]--;
			else
				cornerRot[corners[4]] = 2;

			temp = edges[9];
			edges[9] = edges[7];
			if (edgeRot[edges[9]] == 0)
				edgeRot[edges[9]] = 1;
			else
				edgeRot[edges[9]] = 0;
			edges[7] = edges[11];
			if (edgeRot[edges[7]] == 0)
				edgeRot[edges[7]] = 1;
			else
				edgeRot[edges[7]] = 0;
			edges[11] = edges[10];
			edges[10] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[1];
			corners[1] = corners[4];
			if (cornerRot[corners[1]] != 2)
				cornerRot[corners[1]]++;
			else
				cornerRot[corners[1]] = 0;
			corners[4] = corners[7];
			if (cornerRot[corners[4]] != 0)
				cornerRot[corners[4]]--;
			else
				cornerRot[corners[4]] = 2;
			corners[7] = corners[2];
			if (cornerRot[corners[7]] != 2)
				cornerRot[corners[7]]++;
			else
				cornerRot[corners[7]] = 0;
			corners[2] = (cornerID)temp;
			if (cornerRot[corners[2]] != 0)
				cornerRot[corners[2]]--;
			else
				cornerRot[corners[2]] = 2;

			temp = edges[9];
			edges[9] = edges[10];
			edges[10] = edges[11];
			edges[11] = edges[7];
			if (edgeRot[edges[11]] == 0)
				edgeRot[edges[11]] = 1;
			else
				edgeRot[edges[11]] = 0;
			edges[7] = (edgeID)temp;
			if (edgeRot[edges[7]] == 0)
				edgeRot[edges[7]] = 1;
			else
				edgeRot[edges[7]] = 0;
		}
		break;
	case RED:
		if (ccw == true)
		{
			unsigned temp = corners[5];
			corners[5] = corners[0];
			if (cornerRot[corners[5]] != 0)
				cornerRot[corners[5]]--;
			else
				cornerRot[corners[5]] = 2;
			corners[0] = corners[1];
			if (cornerRot[corners[0]] != 2)
				cornerRot[corners[0]]++;
			else
				cornerRot[corners[0]] = 0;
			corners[1] = corners[4];
			if (cornerRot[corners[1]] != 0)
				cornerRot[corners[1]]--;
			else
				cornerRot[corners[1]] = 2;
			corners[4] = (cornerID)temp;
			if (cornerRot[corners[4]] != 2)
				cornerRot[corners[4]]++;
			else
				cornerRot[corners[4]] = 0;

			temp = edges[2];
			edges[2] = edges[11];
			edges[11] = edges[8];
			edges[8] = edges[5];
			edges[5] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[5];
			corners[5] = corners[4];
			if (cornerRot[corners[5]] != 0)
				cornerRot[corners[5]]--;
			else
				cornerRot[corners[5]] = 2;
			corners[4] = corners[1];
			if (cornerRot[corners[4]] != 2)
				cornerRot[corners[4]]++;
			else
				cornerRot[corners[4]] = 0;
			corners[1] = corners[0];
			if (cornerRot[corners[1]] != 0)
				cornerRot[corners[1]]--;
			else
				cornerRot[corners[1]] = 2;
			corners[0] = (cornerID)temp;
			if (cornerRot[corners[0]] != 2)
				cornerRot[corners[0]]++;
			else
				cornerRot[corners[0]] = 0;

			temp = edges[2];
			edges[2] = edges[5];
			edges[5] = edges[8];
			edges[8] = edges[11];
			edges[11] = (edgeID)temp;
		}
		break;
	case ORANGE:
		if (ccw == true)
		{
			unsigned temp = corners[7];
			corners[7] = corners[2];
			if (cornerRot[corners[7]] != 0)
				cornerRot[corners[7]]--;
			else
				cornerRot[corners[7]] = 2;
			corners[2] = corners[3];
			if (cornerRot[corners[2]] != 2)
				cornerRot[corners[2]]++;
			else
				cornerRot[corners[2]] = 0;
			corners[3] = corners[6];
			if (cornerRot[corners[3]] != 0)
				cornerRot[corners[3]]--;
			else
				cornerRot[corners[3]] = 2;
			corners[6] = (cornerID)temp;
			if (cornerRot[corners[6]] != 2)
				cornerRot[corners[6]]++;
			else
				cornerRot[corners[6]] = 0;

			temp = edges[0];
			edges[0] = edges[3];
			edges[3] = edges[6];
			edges[6] = edges[9];
			edges[9] = (edgeID)temp;
		}
		else
		{
			unsigned temp = corners[7];
			corners[7] = corners[6];
			if (cornerRot[corners[7]] != 0)
				cornerRot[corners[7]]--;
			else
				cornerRot[corners[7]] = 2;
			corners[6] = corners[3];
			if (cornerRot[corners[6]] != 2)
				cornerRot[corners[6]]++;
			else
				cornerRot[corners[6]] = 0;
			corners[3] = corners[2];
			if (cornerRot[corners[3]] != 0)
				cornerRot[corners[3]]--;
			else
				cornerRot[corners[3]] = 2;
			corners[2] = (cornerID)temp;
			if (cornerRot[corners[2]] != 2)
				cornerRot[corners[2]]++;
			else
				cornerRot[corners[2]] = 0;

			temp = edges[0];
			edges[0] = edges[9];
			edges[9] = edges[6];
			edges[6] = edges[3];
			edges[3] = (edgeID)temp;
		}
		break;
	}
}

void RubiksCube::AlignRubiksCube()
{
	//Simply snap rotations to zero degrees
	for (unsigned i = 0; i < 6; i++)
		faceRotation[i] = 0.0f;
}

void RubiksCube::SetFaceColors(GLfloat blockColors[30][3], unsigned face)
{
	faceID faceName = (faceID)face;
	switch (faceName)
	{
	case WHITE:
		SET_WHITE(0);
		break;
	case YELLOW:
		SET_YELLOW(0);
		break;
	case GREEN:
		SET_GREEN(0);
		break;
	case BLUE:
		SET_BLUE(0);
		break;
	case RED:
		SET_RED(0);
		break;
	case ORANGE:
		SET_ORANGE(0);
		break;
	}
}

void RubiksCube::SetCornerColors(GLfloat blockColors[30][3], unsigned corner)
{
	switch (corners[corner])
	{
	case WHITE_RED_GREEN:
		SET_WHITE(0);
		SET_RED(11);
		SET_GREEN(18);
		break;
	case WHITE_RED_BLUE:
		SET_WHITE(0);
		SET_RED(18);
		SET_BLUE(11);
		break;
	case WHITE_ORANGE_BLUE:
		SET_WHITE(0);
		SET_ORANGE(11);
		SET_BLUE(18);
		break;
	case WHITE_ORANGE_GREEN:
		SET_WHITE(0);
		SET_ORANGE(18);
		SET_GREEN(11);
		break;
	case YELLOW_RED_BLUE:
		SET_YELLOW(0);
		SET_RED(11);
		SET_BLUE(18);
		break;
	case YELLOW_RED_GREEN:
		SET_YELLOW(0);
		SET_RED(18);
		SET_GREEN(11);
		break;
	case YELLOW_ORANGE_GREEN:
		SET_YELLOW(0);
		SET_ORANGE(11);
		SET_GREEN(18);
		break;
	case YELLOW_ORANGE_BLUE:
		SET_YELLOW(0);
		SET_ORANGE(18);
		SET_BLUE(11);
		break;
	}
}

void RubiksCube::RotateCorner(GLfloat blockColors[30][3], unsigned state)
{
	GLfloat temp[3];
	switch (state)
	{
	case 1:
		temp[0] = blockColors[0][0]; temp[1] = blockColors[0][1]; temp[2] = blockColors[0][2];
		blockColors[0][0] = blockColors[11][0]; blockColors[0][1] = blockColors[11][1]; blockColors[0][2] = blockColors[11][2];
		blockColors[11][0] = blockColors[18][0]; blockColors[11][1] = blockColors[18][1]; blockColors[11][2] = blockColors[18][2];
		blockColors[18][0] = temp[0]; blockColors[18][1] = temp[1]; blockColors[18][2] = temp[2];
		break;
	case 2:
		temp[0] = blockColors[0][0]; temp[1] = blockColors[0][1]; temp[2] = blockColors[0][2];
		blockColors[0][0] = blockColors[18][0]; blockColors[0][1] = blockColors[18][1]; blockColors[0][2] = blockColors[18][2];
		blockColors[18][0] = blockColors[11][0]; blockColors[18][1] = blockColors[11][1]; blockColors[18][2] = blockColors[11][2];
		blockColors[11][0] = temp[0]; blockColors[11][1] = temp[1]; blockColors[11][2] = temp[2];
		break;
	}
}

void RubiksCube::SetEdgeColors(GLfloat blockColors[30][3], unsigned edge)
{
	switch (edges[edge])
	{
	case WHITE_ORANGE:
		SET_WHITE(0);
		SET_ORANGE(11);
		break;
	case WHITE_GREEN:
		SET_WHITE(0);
		SET_GREEN(11);
		break;
	case WHITE_RED:
		SET_WHITE(0);
		SET_RED(11);
		break;
	case ORANGE_GREEN:
		SET_GREEN(0);
		SET_ORANGE(11);
		break;
	case YELLOW_GREEN:
		SET_GREEN(0);
		SET_YELLOW(11);
		break;
	case RED_GREEN:
		SET_GREEN(0);
		SET_RED(11);
		break;
	case YELLOW_ORANGE:
		SET_YELLOW(0);
		SET_ORANGE(11);
		break;
	case YELLOW_BLUE:
		SET_YELLOW(0);
		SET_BLUE(11);
		break;
	case YELLOW_RED:
		SET_YELLOW(0);
		SET_RED(11);
		break;
	case ORANGE_BLUE:
		SET_BLUE(0);
		SET_ORANGE(11);
		break;
	case WHITE_BLUE:
		SET_BLUE(0);
		SET_WHITE(11);
		break;
	case RED_BLUE:
		SET_BLUE(0);
		SET_RED(11);
		break;
	}
}

void RubiksCube::RotateEdge(GLfloat blockColors[30][3], unsigned state)
{
	GLfloat temp[3];
	switch (state)
	{
	case 1:
		temp[0] = blockColors[0][0]; temp[1] = blockColors[0][1]; temp[2] = blockColors[0][2];
		blockColors[0][0] = blockColors[11][0]; blockColors[0][1] = blockColors[11][1]; blockColors[0][2] = blockColors[11][2];
		blockColors[11][0] = temp[0]; blockColors[11][1] = temp[1]; blockColors[11][2] = temp[2];
		break;
	}
}

#undef SET_WHITE
#undef SET_YELLOW
#undef SET_GREEN
#undef SET_BLUE
#undef SET_RED
#undef SET_ORANGE
#undef SET_BLACK
