#ifndef UITILITY_H
#define UITILITY_H

bool LoadShaderType(char* _filename, GLenum _shader_Type, unsigned int* _output);
bool LoadShader(char* _vertex_Filename,  char* _geometry_Filename, char* _fragment_Filename, GLuint* _result);

#endif