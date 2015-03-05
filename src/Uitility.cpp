#include <cstdio>
#include "gl_core_4_4.h"
#include "Uitility.h"


bool LoadShaderType(char* _filename, GLenum _shader_Type, unsigned int* _output)
{
	bool succeeded = true;

	FILE* shader_File = fopen(_filename, "r");

	if (shader_File == 0)
	{
		succeeded = false;
	}
	else
	{
		fseek(shader_File, 0, SEEK_END);
		int shader_File_Len = ftell(shader_File);
		fseek(shader_File, 0, SEEK_SET);

		char* shader_Source = new char[shader_File_Len];
		shader_File_Len = fread(shader_Source, 1, shader_File_Len, shader_File);

		succeeded = true;
		int success = GL_FALSE;
		int log_len = 0;

		unsigned int shader_Handler = glCreateShader(_shader_Type);

		glShaderSource(shader_Handler, 1, (const char**)&shader_Source, &shader_File_Len);
		glCompileShader(shader_Handler);

		glGetShaderiv(shader_Handler, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			succeeded = false;
			int info_Log_Len = 0;
			glGetShaderiv(shader_Handler, GL_INFO_LOG_LENGTH, &info_Log_Len);

			char* info_Log = new char[info_Log_Len];

			glGetShaderInfoLog(shader_Handler, info_Log_Len, 0, info_Log);

			printf("Error: Shader\n");
			printf("%s \n", info_Log);

			delete[] info_Log;

			fclose(shader_File);
		}
		if (succeeded)
		{
			*_output = shader_Handler;
		}
	}
	return succeeded;
	
}

bool LoadShader(char* _vertex_Filename,  char* _geometry_Filename, char* _fragment_Filename, GLuint* _result)
{
	bool succeeded = false;

	*_result = glCreateProgram();

	unsigned int vertex_Shader;
	
	LoadShaderType(_vertex_Filename, GL_VERTEX_SHADER, &vertex_Shader);
	glAttachShader(*_result, vertex_Shader);
	glDeleteShader(vertex_Shader);

	
	if (_geometry_Filename != nullptr)
	{
		unsigned int geometry_Shader;

		LoadShaderType(_geometry_Filename, GL_GEOMETRY_SHADER, &geometry_Shader);
		glAttachShader(*_result, geometry_Shader);
		glDeleteShader(geometry_Shader);
	}
	if (_fragment_Filename != nullptr)
	{
		unsigned int fragment_Shader;

		LoadShaderType(_fragment_Filename, GL_FRAGMENT_SHADER, &fragment_Shader);
		glAttachShader(*_result, fragment_Shader);
		glDeleteShader(fragment_Shader);
	}
	
	glLinkProgram(*_result);

	GLint success;

	// ERROR CHECKING HERE
	glGetProgramiv(*_result, GL_LINK_STATUS, &success);

	/*if (success == GL_FALSE)
		{
			glGetShaderiv(*result, GL_INFO_LOG_LENGTH, &log_len);

			char* log = new char[log_len];

			glGetShaderInfoLog(vertex_shader, log_len, NULL, log);

			printf("%s\n", log);

			return false;

			delete[] log;
		}*/

	if (success == GL_FALSE)
	{
		GLint log_Len;

		glGetProgramiv(*_result, GL_INFO_LOG_LENGTH, &log_Len);

		char* log = new char[log_Len];
		glGetProgramInfoLog(*_result, log_Len, 0, log);

		printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n");
		printf("%s", log);

		delete[] log;
		return succeeded = false;
	}
	
	return succeeded;
}