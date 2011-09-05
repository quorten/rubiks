#ifndef RUBIKS_H
#define RUBIKS_H

enum faceID
{
	WHITE,
	YELLOW,
	GREEN,
	BLUE,
	RED,
	ORANGE
};

class RubiksCube
{
public:
	RubiksCube();
	~RubiksCube();

	void NewRubiksCube();
	void Init();
	void Prepare(float dt);
	void Render();
	void FindMousePick(int x, int y);
	void RotateFace(int x, int y);
	void AlignRubiksCube();
	void RotateFace90(bool ccw); //counterclockwise
	void SetFace(unsigned face) { currentFace = face; }
	bool IsRubiksReady() { return rubiksReady; }
private:
	bool rubiksReady; //To make sure that Render() is not called before Init()

	//Piece position info
	enum cornerID
	{
		WHITE_RED_GREEN,
		WHITE_RED_BLUE,
		WHITE_ORANGE_BLUE,
		WHITE_ORANGE_GREEN,
		YELLOW_RED_BLUE,
		YELLOW_RED_GREEN,
		YELLOW_ORANGE_GREEN,
		YELLOW_ORANGE_BLUE
	};
	enum edgeID
	{
		WHITE_ORANGE,
		WHITE_GREEN,
		WHITE_RED,
		ORANGE_GREEN,
		YELLOW_GREEN,
		RED_GREEN,
		YELLOW_ORANGE,
		YELLOW_BLUE,
		YELLOW_RED,
		ORANGE_BLUE,
		WHITE_BLUE,
		RED_BLUE
	};
	float faceRotation[6]; //In degrees
	unsigned currentFace; //Specifies current custom rotated face
	Point oldPick; //Stores last position used for custom rotation
	cornerID corners[8];
	unsigned cornerRot[8]; //Specifies a rotation state - 3 possible
	edgeID edges[12];
	unsigned edgeRot[12]; //Specifies a rotation state - 2 possible

	void SetFaceColors(GLfloat blockColors[30][3], unsigned face);
	void SetCornerColors(GLfloat blockColors[30][3], unsigned corner);
	void RotateCorner(GLfloat blockColors[30][3], unsigned state);
	void SetEdgeColors(GLfloat blockColors[30][3], unsigned edge);
	void RotateEdge(GLfloat blockColors[30][3], unsigned state);
};

#endif
