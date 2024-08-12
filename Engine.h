#ifndef ENGINE_H
#define ENGINE_H

#include "Library.h"

#define FILEPATH const char*

struct SQUI { QPlainTextEdit* plainTextEdit; };

struct VertexAttri { float x; float y; float z; };
struct VertexIndex { int posIndex; int texIndex; int norIndex; };

class Engine {
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

private:
	//Var
	QPlainTextEdit* plainTextEdit;
private:
	//Fun


};

#endif // ENGINE_H
