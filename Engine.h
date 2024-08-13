#ifndef ENGINE_H
#define ENGINE_H

#include "Library.h"
#include "Camera.h"

#define FILEPATH const char*

struct SQUI { QPlainTextEdit* plainTextEdit; };

struct VertexAttri { float x; float y; float z; };
struct VertexIndex { int posIndex; int texIndex; int norIndex; };

class Engine : public QOpenGLFunctions_3_3_Core {
public:
	Engine(SQUI qui);
	~Engine();

	//Log
	enum LogLevel { ERR, WARN, INFO, SUC };
	void Log(LogLevel loglevel, QString text);

	//Tools
	char* LoadFileContext(FILEPATH path);

	//OBJ
	struct Vertex { float position[3]; float normal[3]; float texcoord[2]; };
	struct Mesh { uint32_t indexCount; uint32_t* indices; uint32_t vertexCount; Vertex* vertices; uint32_t faceCont; };
	Mesh* LoadObjModel(FILEPATH url, bool isDropRepeat);
	//Shader
	void InitializeShader(const char* vs,const char *fs);
	GLuint CompileShader(GLenum shaderType, const char* url);
	GLuint CreateGPUProgram(const char* vs, const char* fs);
	GLuint CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data);
	GLuint CreateGLTexture(GLenum target, GLint internalformat, GLint format, QImage img);
	void ShaderProgram(const char* modle, const char* textrue);
	void GLSetting();
	void GLPaint(Camera* camera, int glwidth, int glheight);

public:
	GLuint shaderprogram;
	GLuint smp;
	GLuint modelLocation;
	GLuint viewlLocation;
	GLuint projlLocation;
	struct Mesh* mesh = NULL;
	GLuint tex;
	GLuint VBO, EBO, VAO;
	Camera *camera;

private:
	QPlainTextEdit* plainTextEdit;
	
};

#endif // ENGINE_H
