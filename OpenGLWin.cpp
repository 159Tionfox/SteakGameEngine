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
	engine->Log(Engine::LogLevel::SUC, "Initialize QOpenGL Success");

	engine->InitializeShader("Engine/Shader/NorVertexShader.glsl", "Engine/Shader/NorFragmentShader.glsl");
	engine->ShaderProgram("Engine/Model/Plane.obj", "Engine/Texture/b.png");

	engine->GLSetting();

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
	engine->GLPaint(&camera, glwidth, glheight);

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
		camera.cameraspeed += 0.5;
		engine->Log(Engine::LogLevel::INFO, QString::number(camera.cameraspeed));
	}
	else
	{
		if (right)
		{
			camera.cameraspeed -= 0.5;
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