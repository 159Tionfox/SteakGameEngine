#ifndef OPENGLWIN_H
#define OPENGLWIN_H

#include "Engine.h"
#include "Library.h"
#include "Camera.h"

class OpenGLWin : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	explicit OpenGLWin(QPlainTextEdit* pte, QWidget* parent = nullptr);
	~OpenGLWin();

	Engine* engine;

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	//Shader
	GLuint CompileShader(GLenum shaderType, const char* url);
	GLuint CreateGPUProgram(const char* vs, const char* fs);
	GLuint CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data);
	GLuint CreateGLTexture(GLenum target, GLint internalformat, GLint format, QImage img);


protected:
	Camera camera;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	bool pressW = false, pressS = false, pressA = false, pressD = false, pressQ = false, pressE = false;
	QPoint lastpoint;
	bool right = false;

private:
	struct Engine::Mesh* mesh = NULL;

	GLuint VBO, EBO, VAO;
	GLuint ShaderProgram;
	GLuint tex;
	GLuint smp;
	GLuint modelLocation;
	GLuint viewlLocation;
	GLuint projlLocation;

private:
	QPlainTextEdit* pte;
	QTimer* timer;
	int frameRate;
	int glwidth, glheight;

private slots:
	void tick();
};


#endif	//OPENGLWIN_H
