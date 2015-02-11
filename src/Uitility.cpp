#include <cstdio>
#include "gl_core_4_4.h"
#include "Uitility.h"

bool Uitility::LoadShader(char* vertex_filename, char* fragment_filename, GLuint* result)
{
	bool succeeded = false;

	FILE* vertex_file = fopen(vertex_filename, "r");
	FILE* fragment_file = fopen(fragment_filename, "r");

	if (vertex_file == 0 || fragment_file == 0)
	{

	}
	else
	{
		fseek(vertex_file, 0, SEEK_END);
		int vertex_file_len = ftell(vertex_file);
		fseek(vertex_file, 0, SEEK_SET);

		fseek(fragment_file, 0, SEEK_END);
		int fragment_file_len = ftell(fragment_file);
		fseek(fragment_file, 0, SEEK_SET);

		char* vs_source = new char[vertex_file_len];
		char* fs_source = new char[fragment_file_len];

		vertex_file_len = fread(vs_source, 1, vertex_file_len, vertex_file);
		fragment_file_len = fread(fs_source, 1, fragment_file_len, fragment_file);

		succeeded = true;
		int success = GL_FALSE;
		int log_len = 0;

		unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex_shader, 1, (const char**)&vs_source, 0);
		glCompileShader(vertex_shader);

		glShaderSource(fragment_shader, 1, (const char**)&fs_source, 0);
		glCompileShader(fragment_shader);

		*result = glCreateProgram();
		glAttachShader(*result, vertex_shader);
		glAttachShader(*result, fragment_shader);
		glLinkProgram(vertex_shader);

		// ERROR CHECKING HERE
		glGetProgramiv(*result, GL_LINK_STATUS, &success);

		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_len);

			char* log = new char[log_len];

			glGetShaderInfoLog(vertex_shader, log_len, NULL, log);

			printf("%s\n", log);

			succeeded = false;

			delete[] log;
		}

		if (success == GL_FALSE)
		{
			glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_len);

			char* log = new char[log_len];
			glGetProgramInfoLog(*result, log_len, 0, log);

			printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n");
			printf("%s", log);

			succeeded = false;

			delete[] log;
		}

		delete vs_source;
		delete fs_source;

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);

		return succeeded;
	}
}