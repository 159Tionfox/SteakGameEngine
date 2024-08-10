#ifndef OPENGLWIN_H
#define OPENGLWIN_H

#include "Tools.h"
#include "Library.h"

class OpenGLWin : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	explicit OpenGLWin(QPlainTextEdit* pte, QWidget* parent = nullptr);
	~OpenGLWin();

protected:
	virtual void initializeGL();
	//virtual void resizeGL(int w,int h);
	virtual void paintGL();

private:
	GLuint CompileShader(GLenum shaderType, const char* url);

private:
	QPlainTextEdit* pte;
	QTimer* timer;
	int frameRate;


	float vertices[9] = {
		 0.0f, 0.5f, 0.0f,
		-0.5f,-0.5f, 0.0f,
		 0.5f,-0.5f, 0.0f
	};

	GLuint VBO;

private slots:
	void tick();
};


#endif	//OPENGLWIN_H
