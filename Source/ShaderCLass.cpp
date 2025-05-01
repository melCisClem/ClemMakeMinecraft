#include "../Headers/ShaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);
	if (ifs)
	{
		std::string contents;

		ifs.seekg(0, std::ios::end);
		contents.resize(ifs.tellg());

		ifs.seekg(0, std::ios::beg);
		ifs.read(&contents[0], contents.size());

		ifs.close();
		return contents;
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

    int  success;
    char infoLog[512];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    glGetShaderiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}


void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}


void Shader::compileErrors(unsigned int shader, const char* type)
{
    GLint hasCompiled;
    char infolog[1024];
    
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (!hasCompiled)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infolog);
            std::cout << "SHADER COMPILATION ERROR for: " << type << '\n' << infolog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (!hasCompiled)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infolog);
            std::cout << "SHADER LINKING ERROR for: " << type << '\n' << infolog << std::endl;
        }
    }
}
