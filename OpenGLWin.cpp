#include "OpenGLWin.h"

OpenGLWin::OpenGLWin(QPlainTextEdit* pte, QWidget* parent) : QOpenGLWidget(parent)
{
	SQUI squi;
	squi.plainTextEdit = pte;
	engine = new Engine(squi);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
	this->pte = pte;
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &OpenGLWin::tick);
	timer->start(1000 / frameRate);

	camera.SetRotation(0, 180, 0);
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
	if (!initializeOpenGLFunctions()) {
		engine->Log(Engine::LogLevel::ERR, "Initialize QOpenGL Error!");
	}

	//QDesktopServices::openUrl(QUrl(QLatin1String("https://www.google.com/")));

	engine->Log(Engine::LogLevel::SUC, "Initialize QOpenGL Success");

	ShaderProgram = CreateGPUProgram("Engine/Shader/NorVertexShader.glsl", "Engine/Shader/NorFragmentShader.glsl");
	GLint PosLocation = glGetAttribLocation(ShaderProgram, "Pos");
	GLint ColorLocation = glGetAttribLocation(ShaderProgram, "Color");
	GLint TexCoordLocation = glGetAttribLocation(ShaderProgram, "TexCoord");

	smp = glGetUniformLocation(ShaderProgram, "smp");

	modelLocation = glGetUniformLocation(ShaderProgram, "ModelMat");
	viewlLocation = glGetUniformLocation(ShaderProgram, "ViewMat");
	projlLocation = glGetUniformLocation(ShaderProgram, "ProjMat");

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	mesh = engine->LoadObjModel("Engine/Model/xm177gun.obj",true);

	VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->vertexCount * sizeof(Engine::Vertex), mesh->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(PosLocation);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(ColorLocation);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(TexCoordLocation);
	glVertexAttribPointer(TexCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->indexCount * sizeof(uint32_t), mesh->indices);

	tex = CreateGLTexture(GL_TEXTURE_2D, GL_RGBA, GL_BGRA, QImage("Engine/Texture/b.jpg"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	GLenum errorcode = glGetError();
	if (errorcode) {
		engine->Log(Engine::LogLevel::ERR, "OpenGLError:\t" + QString::number(errorcode));
	}
}

void OpenGLWin::resizeGL(int w, int h)
{
	glwidth = w;
	glheight = h;
}

void OpenGLWin::paintGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(ShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(smp, 0);

	QMatrix4x4 modelMat;
	QMatrix4x4 projMat;

	modelMat.translate(0, 0, -3);
	modelMat.scale(1, 1, 1);
	projMat.perspective(45, glwidth / (float)glheight, 0.1f, 100);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelMat.constData());
	glUniformMatrix4fv(viewlLocation, 1, GL_FALSE, camera.GetViewMat().constData());
	glUniformMatrix4fv(projlLocation, 1, GL_FALSE, projMat.constData());

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT,NULL);
}

GLuint OpenGLWin::CompileShader(GLenum shaderType, const char* url)
{
	char* shaderCode = engine->LoadFileContext(url);
	const char* shaderTypeStr = "Vertex Shader";
	if (shaderType == GL_FRAGMENT_SHADER) shaderTypeStr = "Fragment Shader";
	GLuint shader = glCreateShader(shaderType);
	if (!shader) {
		engine->Log(Engine::LogLevel::ERR, "Create Shader Failed");
	}

	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	GLint success = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infolog[1024];
		GLsizei loglen = 0;
		glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
		engine->Log(Engine::LogLevel::ERR, "Compile " + QString(shaderTypeStr) +" error: " + QString(infolog));
		glDeleteShader(shader);
	}
	engine->Log(Engine::LogLevel::SUC, "Compile " + QString(shaderTypeStr) + " Success");
	delete shaderCode;
	return shader;
}

GLuint OpenGLWin::CreateGPUProgram(const char* vs, const char* fs)
{
	GLuint vss = CompileShader(GL_VERTEX_SHADER, vs);
	GLuint fss = CompileShader(GL_FRAGMENT_SHADER, fs);

	GLuint m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vss);
	glAttachShader(m_shaderProgram, fss);
	glLinkProgram(m_shaderProgram);

	GLint success = GL_TRUE;
	glGetShaderiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infolog[1024];
		GLsizei loglen = 0;
		glGetShaderInfoLog(m_shaderProgram, sizeof(infolog), &loglen, infolog);
		engine->Log(Engine::LogLevel::ERR, "Link ShaderProgram error: " + QString(infolog));
		glDeleteShader(m_shaderProgram);
	}
	engine->Log(Engine::LogLevel::SUC, "Link ShaderProgram Success");
	glDetachShader(m_shaderProgram, vss);
	glDetachShader(m_shaderProgram, fss);
	glDeleteShader(vss);
	glDeleteShader(fss);
	return m_shaderProgram;
}

GLuint OpenGLWin::CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data)
{
	GLuint bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(target, bufferId);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
	return bufferId;
}

GLuint OpenGLWin::CreateGLTexture(GLenum target, GLint internalformat, GLint format, QImage img)
{
	GLuint texid;
	glGenTextures(1, &texid);
	glBindTexture(target, texid);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(target, 0, internalformat, img.width(), img.height(), 0, format, GL_UNSIGNED_BYTE, img.bits());
	glGenerateMipmap(target);

	return texid;
}

void OpenGLWin::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_W) pressW = true;
	if (event->key() == Qt::Key_S) pressS = true;
	if (event->key() == Qt::Key_A) pressA = true;
	if (event->key() == Qt::Key_D) pressD = true;
	if (event->key() == Qt::Key_Q) pressQ = true;
	if (event->key() == Qt::Key_E) pressE = true;
}

void OpenGLWin::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_W) pressW = false;
	if (event->key() == Qt::Key_S) pressS = false;
	if (event->key() == Qt::Key_A) pressA = false;
	if (event->key() == Qt::Key_D) pressD = false;
	if (event->key() == Qt::Key_Q) pressQ = false;
	if (event->key() == Qt::Key_E) pressE = false;
}

void OpenGLWin::mousePressEvent(QMouseEvent* event)
{
	lastpoint = event->pos();
	if (event->button() == Qt::RightButton) {
		right = true;
	}
}

void OpenGLWin::mouseMoveEvent(QMouseEvent* event)
{
	if (right) {
		QPoint delta = event->pos() - lastpoint;
		lastpoint = event->pos();
		camera.Rotate(-delta.y() * camera.camerapitch, -delta.x() * camera.camerayaw, 0);
	}

	update();
}

void OpenGLWin::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton) {
		right = false;
	}
}

void OpenGLWin::wheelEvent(QWheelEvent* event)
{
	int delta = event->angleDelta().y();
	if (delta > 0 && right) {
		camera.cameraspeed += 0.1;
		engine->Log(Engine::LogLevel::INFO, QString::number(camera.cameraspeed));
	}
	else
	{
		if (right)
		{
			camera.cameraspeed -= 0.1;
			engine->Log(Engine::LogLevel::INFO, QString::number(camera.cameraspeed));
		}
	}
}

void OpenGLWin::tick() {

	static long long lastts = 0;
	if (lastts == 0) lastts = QDateTime::currentMSecsSinceEpoch();
	float dt = QDateTime::currentMSecsSinceEpoch() - lastts;
	lastts = QDateTime::currentMSecsSinceEpoch();
	dt /= 1000;

	if (pressW) camera.Position(0, 0, camera.cameraspeed * dt);
	if (pressS) camera.Position(0, 0, -camera.cameraspeed * dt);
	if (pressD) camera.Position(camera.cameraspeed * dt, 0, 0);
	if (pressA) camera.Position(-camera.cameraspeed * dt, 0, 0);
	if (pressQ) camera.Position(0, -camera.cameraspeed * dt, 0);
	if (pressE) camera.Position(0, camera.cameraspeed * dt, 0);

	update();
}