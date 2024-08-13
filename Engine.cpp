#include "Engine.h"

Engine::Engine(SQUI qui)
{
    if (qui.plainTextEdit) {
        this->plainTextEdit = qui.plainTextEdit;
    }
}

Engine::~Engine()
{
    if (this->plainTextEdit) delete this->plainTextEdit;
}

void Engine::Log(LogLevel loglevel, QString text)
{
    QTextCharFormat fmt;
    QString str;
    switch (loglevel)
    {
    case ERR:
        fmt.setForeground(QBrush(QColor(252, 0, 0)));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | ERROR] ") + text;
        break;
    case WARN:
        fmt.setForeground(QBrush(QColor("yellow")));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | WARNING] ") + text;
        break;
    case INFO:
        fmt.setForeground(QBrush(QColor("white")));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | INFO] ") + text;
        break;
    case SUC:
        fmt.setForeground(QBrush(QColor(0, 252, 0)));
        str = QDateTime::currentDateTime().toString("[hh:mm:ss | SUCCESS] ") + text;
        break;
    }
    plainTextEdit->mergeCurrentCharFormat(fmt);
    plainTextEdit->appendPlainText(str);
}

char* Engine::LoadFileContext(FILEPATH path)
{
    char* file = NULL;
    FILE* fp = fopen(path, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        int len = ftell(fp);
        if (len > 0) {
            rewind(fp);
            file = new char[len + 1];
            fread(file, 1, len, fp);
            file[len] = 0;
        }
        fclose(fp);
    }
    return file;
}

Engine::Mesh* Engine::LoadObjModel(FILEPATH url, bool isDropRepeat)
{
	char* fileContext = LoadFileContext(url);
	if (!fileContext)
		return NULL;

	std::vector<VertexAttri> positions, normals, texcoords;
	std::vector<VertexIndex> vertices;
	std::vector<uint32_t> indices;
	std::stringstream strStream(fileContext);
	delete fileContext;

	std::string temp;
	while (!strStream.eof())
	{
		const int MAX_SIZE = 256;
		char lineStr[MAX_SIZE];
		strStream.getline(lineStr, MAX_SIZE);

		std::stringstream lineStream(lineStr);
		if (lineStr[0] == 'v')
		{
			if (lineStr[1] == 't')
			{
				lineStream >> temp;
				VertexAttri attri;
				lineStream >> attri.x;
				lineStream >> attri.y;
				lineStream >> attri.z;
				texcoords.push_back(attri);
			}
			else if (lineStr[1] == 'n')
			{
				lineStream >> temp;
				VertexAttri attri;
				lineStream >> attri.x;
				lineStream >> attri.y;
				lineStream >> attri.z;
				normals.push_back(attri);
			}
			else
			{
				lineStream >> temp;
				VertexAttri attri;
				lineStream >> attri.x;
				lineStream >> attri.y;
				lineStream >> attri.z;
				positions.push_back(attri);
			}
		}
		else if (lineStr[0] == 'f')
		{
			lineStream >> temp;
			for (int i = 0; i < 3; i++)
			{
				lineStream >> temp;
				int pos1 = temp.find_first_of('/');
				std::string vStr = temp.substr(0, pos1);
				int pos2 = temp.find_first_of('/', pos1 + 1);
				std::string tStr = temp.substr(pos1 + 1, pos2 - pos1 - 1);
				std::string nStr = temp.substr(pos2 + 1, temp.length() - pos2 - 1);

				VertexIndex vi;
				vi.posIndex = atoi(vStr.c_str()) - 1;
				vi.texIndex = atoi(tStr.c_str()) - 1;
				vi.norIndex = atoi(nStr.c_str()) - 1;

				int index = -1;
				if (isDropRepeat)
				{
					int currentVertexCount = vertices.size();
					for (int k = 0; k < currentVertexCount; k++)
					{
						if (vertices[k].posIndex == vi.posIndex &&
							vertices[k].norIndex == vi.norIndex &&
							vertices[k].texIndex == vi.texIndex)
						{
							index = k;
							break;
						}
					}
				}
				if (index == -1)
				{
					index = vertices.size();
					vertices.push_back(vi);
				}
				indices.push_back(index);
			}
		}
	}
	Mesh* mesh = new Mesh();
	mesh->indexCount = indices.size();
	mesh->indices = new uint32_t[mesh->indexCount];
	memcpy(mesh->indices, &indices[0], mesh->indexCount * sizeof(uint32_t));

	mesh->vertexCount = vertices.size();
	mesh->vertices = new Vertex[mesh->vertexCount];
	for (int i = 0; i < mesh->vertexCount; i++)
	{
		memcpy(&mesh->vertices[i].position, &positions[vertices[i].posIndex], sizeof(float) * 3);
		memcpy(&mesh->vertices[i].normal, &normals[vertices[i].norIndex], sizeof(float) * 3);
		memcpy(&mesh->vertices[i].texcoord, &texcoords[vertices[i].texIndex], sizeof(float) * 2);
	}
	mesh->faceCont = mesh->indexCount / 3;
	return mesh;
}

void Engine::InitializeShader(const char* vs, const char* fs)
{
	if (!initializeOpenGLFunctions()) {
		Log(Engine::LogLevel::ERR, "Initialize QOpenGL Error!");
	}

	shaderprogram = CreateGPUProgram(vs, fs);
}

GLuint Engine::CompileShader(GLenum shaderType, const char* url)
{

	char* shaderCode = LoadFileContext(url);
	const char* shaderTypeStr = "Vertex Shader";
	if (shaderType == GL_FRAGMENT_SHADER) shaderTypeStr = "Fragment Shader";
	GLuint shader = glCreateShader(shaderType);
	if (!shader) {
		Log(Engine::LogLevel::ERR, "Create Shader Failed");
	}

	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	GLint success = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infolog[1024];
		GLsizei loglen = 0;
		glGetShaderInfoLog(shader, sizeof(infolog), &loglen, infolog);
		Log(Engine::LogLevel::ERR, "Compile " + QString(shaderTypeStr) + " error: " + QString(infolog));
		glDeleteShader(shader);
	}
	Log(Engine::LogLevel::SUC, "Compile " + QString(shaderTypeStr) + " Success");
	delete shaderCode;
	return shader;
}

GLuint Engine::CreateGPUProgram(const char* vs, const char* fs)
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
		Log(Engine::LogLevel::ERR, "Link ShaderProgram error: " + QString(infolog));
		glDeleteShader(m_shaderProgram);
	}
	Log(Engine::LogLevel::SUC, "Link ShaderProgram Success");
	glDetachShader(m_shaderProgram, vss);
	glDetachShader(m_shaderProgram, fss);
	glDeleteShader(vss);
	glDeleteShader(fss);
	return m_shaderProgram;
}

GLuint Engine::CreateGLBuffer(GLenum target, GLenum usage, GLsizeiptr size, const void* data)
{
	GLuint bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(target, bufferId);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
	return bufferId;
}

GLuint Engine::CreateGLTexture(GLenum target, GLint internalformat, GLint format, QImage img)
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

void Engine::ShaderProgram(const char* modle, const char* textrue)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	mesh = LoadObjModel(modle, true);

	VBO = CreateGLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->vertexCount * sizeof(Engine::Vertex), mesh->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLint PosLocation = glGetAttribLocation(shaderprogram, "Pos");
	GLint ColorLocation = glGetAttribLocation(shaderprogram, "Color");
	GLint TexCoordLocation = glGetAttribLocation(shaderprogram, "TexCoord");

	smp = glGetUniformLocation(shaderprogram, "smp");

	modelLocation = glGetUniformLocation(shaderprogram, "ModelMat");
	viewlLocation = glGetUniformLocation(shaderprogram, "ViewMat");
	projlLocation = glGetUniformLocation(shaderprogram, "ProjMat");

	glEnableVertexAttribArray(PosLocation);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(ColorLocation);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(TexCoordLocation);
	glVertexAttribPointer(TexCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	EBO = CreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->indexCount * sizeof(uint32_t), mesh->indices);
	tex = CreateGLTexture(GL_TEXTURE_2D, GL_RGBA, GL_BGRA, QImage(textrue));
}

void Engine::GLSetting()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
}

void Engine::GLPaint(Camera* camera, int glwidth, int glheight)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderprogram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(smp, 0);

	QMatrix4x4 modelMat;
	QMatrix4x4 projMat;

	modelMat.translate(0, 0, -3);
	modelMat.scale(1, 1, 1);
	projMat.perspective(45, glwidth / (float)glheight, 0.1f, 100);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, modelMat.constData());
	glUniformMatrix4fv(viewlLocation, 1, GL_FALSE, camera->GetViewMat().constData());
	glUniformMatrix4fv(projlLocation, 1, GL_FALSE, projMat.constData());

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL);
}
