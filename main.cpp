#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define GLEW_BUILD


struct TSharedProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static TSharedProgramSource ParseShader(const std::string& filePath)
{

	enum class ShaderType 
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filePath);
	ShaderType shType;
	std::string line;
	std::stringstream ss[2];

	if(stream.is_open() == false)
		std::cout <<"Error , file not found !  "<< std::endl;

	while(getline(stream, line))
	{		
		if(line.find("#shader") != std::string::npos)
		{
			if(line.find("vertex") != std::string::npos)
				shType = ShaderType::VERTEX;
			else if(line.find("fragment") != std::string::npos)
			{
				shType = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)shType] << line << "\n";
		}
	}

	return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShared(unsigned int type, const std::string& source)
{	
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE)
	{		
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout <<"Compile failed for " << (type == GL_VERTEX_SHADER ? "vertex shared" : "fragment shader") << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShared(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShared(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{

	GLFWwindow* window;

	if(!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(640,480, "Salut lume", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return -1;

	}

	glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK)
		std::cout <<"Error , glew not ok init" << std::endl;	

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions [] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,		 		 
		-0.5f,  0.5f
	};

	//Index buffer
	unsigned int indices [] = {
		0,1,2,
		2,3,0
	};

	unsigned int buffer;
	glGenBuffers(1,&buffer);
	glBindBuffer(GL_ARRAY_BUFFER,buffer);
	glBufferData(GL_ARRAY_BUFFER,6*2*sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
	

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	 
	//unsigned int shader = CreateShader(vertexShader, fragmentShader);
	TSharedProgramSource source = ParseShader("./res/shaders/Basic.shader");
	std::cout << source.VertexSource << std::endl;
	std::cout << source.FragmentSource << std::endl;

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);		//state-setting function
		glClear(GL_COLOR_BUFFER_BIT);				//state-using function , uses current state to retrive the clearing color from

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,nullptr);
		
		//Double buffering method
		glfwSwapBuffers(window);

		glfwPollEvents();

		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
	}

	//glDeleteProgram(program);

	glfwTerminate();

	return 0;
}
