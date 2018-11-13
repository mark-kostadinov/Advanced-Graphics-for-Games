#include "Shader.h"

Shader::Shader(string vFile, string fFile, string tcsFile, string tesFile, string gFile)
{
	vertexName = vFile;
	fragName = fFile;
	tcsName = tcsFile;
	tesName = tesFile;
	geomName = gFile;

	Reload(false);
}

Shader::~Shader(void)
{
	for (int i = 0; i < SHADER_COUNT; ++i)
	{
		glDetachShader(program, objects[i]);
		glDeleteShader(objects[i]);
	}
	glDeleteProgram(program);
}

void Shader::Reload(bool deleteOld)
{
	if (deleteOld)
	{
		for (int i = 0; i < SHADER_COUNT; ++i)
		{
			glDetachShader(program, objects[i]);
			glDeleteShader(objects[i]);
		}
		glDeleteProgram(program);
	}

	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vertexName, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fragName, GL_FRAGMENT_SHADER);
	objects[SHADER_TESS_CONTROL] = 0;
	objects[SHADER_TESS_EVALUATION] = 0;
	objects[SHADER_GEOMETRY] = 0;

	if (!tcsName.empty())
	{
		objects[SHADER_TESS_CONTROL] = GenerateShader(tcsName, GL_TESS_CONTROL_SHADER);
		glAttachShader(program, objects[SHADER_TESS_CONTROL]);
	}
	if (!tesName.empty())
	{
		objects[SHADER_TESS_EVALUATION] = GenerateShader(tesName, GL_TESS_EVALUATION_SHADER);
		glAttachShader(program, objects[SHADER_TESS_EVALUATION]);
	}
	if (!geomName.empty())
	{
		objects[SHADER_GEOMETRY] = GenerateShader(geomName, GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}
	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	SetDefaultAttributes();
	LinkProgram();
}

bool Shader::LoadShaderFile(string from, string &into)
{
	ifstream	file;
	string		temp;

	cout << "Loading shader text from " << from << endl << endl;

	file.open(from.c_str());
	if (!file.is_open())
	{
		cout << "File does not exist!" << endl;
		return false;
	}

	while (!file.eof())
	{
		getline(file, temp);
		into += temp + "\n";
	}

	cout << into << endl << endl;

	file.close();
	cout << "Loaded shader text!" << endl << endl;
	return true;
}

GLuint Shader::GenerateShader(string from, GLenum type)
{
	cout << "Compiling Shader..." << endl;

	string load;
	if (!LoadShaderFile(from, load))
	{
		cout << "Compiling failed!" << endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char *chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		cout << "Compiling failed!" << endl;
		char error[2048];
		//glGetInfoLogARB(shader, sizeof(error), NULL, error);

		glGetShaderInfoLog(shader, 2048, NULL, error);

		cout << error;
		loadFailed = true;
		return 0;
	}
	cout << "Compiling success!" << endl << endl;
	loadFailed = false;
	return shader;
}

bool Shader::LinkProgram()
{
	if (loadFailed)
		return false;

	glLinkProgram(program);

	GLint status = 1;
	glGetShaderiv(program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Linking failed!" << endl;
		char error[8192];
		//glGetInfoLogARB(program, sizeof(error), NULL, error);

		glGetProgramInfoLog(program, 8192, NULL, error);
		cout << error;
		loadFailed = true;
		return false;
	}
	return true;
}

void Shader::SetDefaultAttributes()
{
	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, NORMAL_BUFFER, "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
}