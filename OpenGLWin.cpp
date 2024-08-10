#include "OpenGLWin.h"

OpenGLWin::OpenGLWin(QPlainTextEdit* pte, QWidget* parent) : QOpenGLWidget(parent)
{
	this->pte = pte;
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &OpenGLWin::tick);
	timer->start(1000 / frameRate);
}

OpenGLWin::~OpenGLWin()
{
	if (timer) {
		timer->stop();
		delete timer;
	}
}

void OpenGLWin::initializeGL()
{
	initializeOpenGLFunctions();
	Log(pte, LogLevel::SUC, "Initialize QOpenGL Success");

	CompileShader(GL_VERTEX_SHADER, "../Engine/Shader/NorVertexShader.glsl");
	CompileShader(GL_FRAGMENT_SHADER, "../Engine/Shader/NorFragmentShader.glsl");

	glGenBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int size = sizeof(vertices);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWin::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

GLuint OpenGLWin::CompileShader(GLenum shaderType, const char* url)
{
	char* shaderCode = LoadFileContext(url);
	const char* shaderTypeStr = "Vertex Shader";
	if (shaderType == GL_FRAGMENT_SHADER) shaderTypeStr = "Fragment Shader";
	GLuint shader = glCreateShader(shaderType);
	if (!shader) {
		Log(pte, LogLevel::ERR, "Create Shader Failed");
	}

	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	GLint success = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infolog[1024];
		GLsizei loglen = 0;
		glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
		Log(pte, LogLevel::ERR, "Compile " + QString(shaderTypeStr) +" error: " + QString(infolog));
		glDeleteShader(shader);
	}
	Log(pte, LogLevel::SUC, "Compile " + QString(shaderTypeStr) + " Success");
	delete shaderCode;
	return shader;
}

void OpenGLWin::tick() {
	update();
}