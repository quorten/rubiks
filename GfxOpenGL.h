#ifndef __GL_COMPONENT
#define __GL_COMPONENT

struct angle
{
	GLfloat x;
	GLfloat y;
};

class GfxOpenGL
{
public:
	GfxOpenGL();
	~GfxOpenGL();

	bool Init();
	void SetupProjection(unsigned width, unsigned height);
	bool Shutdown();
	void Prepare(float dt);
	void Render();
	void RotatingUp(bool movement) { m_xRotateState = (GLfloat)movement * -1.0f; }
	void RotatingDown(bool movement) { m_xRotateState = (GLfloat)movement; }
	void RotatingLeft(bool movement) { m_yRotateState = (GLfloat)movement * -1.0f; }
	void RotatingRight(bool movement) { m_yRotateState = (GLfloat)movement; }
	void SetRotSpeed(bool quadSpeed) { m_quadSpeed = quadSpeed; }
	void MouseRotate(int xScr, int yScr, bool firstTime);

	//angle GetEyeAngle() { return m_eyeAngle; }
	//Screen coordinate conversions
	float ConvScrCoordsX(int coord) const { return (2 * (float)coord - m_windowWidth) / m_windowHeight; }
	float ConvScrCoordsY(int coord) const { return -2 * (float)coord / m_windowHeight + 1.0f; }
	void Get3DPick(int x, int y, unsigned& pickedFace, Point& finalPick);
	void Get3DDeltaPick(int x, int y, unsigned& currentFace, Point& finalPick);

private:
	unsigned m_windowWidth;
	unsigned m_windowHeight;
	GLfloat m_xRotateState;
	GLfloat m_yRotateState;
	bool m_quadSpeed;
	GLfloat rotateMatrix[16]; //Stores rotation matrix
	//angle m_eyeAngle;
};

#endif
