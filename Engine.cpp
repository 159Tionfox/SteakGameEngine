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