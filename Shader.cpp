#include "Shader.h"

Shader::Shader()
{

}


GLuint Shader::shaderCompileFromFile(GLenum type, const char *filePath)
{
    char *source;
    GLint shader;
    GLint length, result;

    /* get shader source */
    source = shaderLoadSource(filePath);
    if(!source)
        return 0;

    /* create shader object, set the source, and compile */
    shader = glCreateShader(type);
    length = strlen(source);
    glShaderSource(shader, 1, (const char **)&source, &length);
    glCompileShader(shader);
    free(source);

    /* make sure the compilation was successful */
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        char *log;

        /* get the shader info log */
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetShaderInfoLog(shader, length, &result, log);

        /* print an error message and the info log */
        fprintf(stderr, "shaderCompileFromFile(): Unable to compile %s: %s\n", filePath, log);
        free(log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


void Shader::shaderAttachFromFile(GLuint program, GLenum type, const char *filePath)
{
	 /* compile the shader */
    GLuint shader = shaderCompileFromFile(type, filePath);
    if(shader != 0) {
        /* attach the shader to the program */
        glAttachShader(program, shader);

        /* delete the shader - it won't actually be
         * destroyed until the program that it's attached
         * to has been destroyed */
        glDeleteShader(shader);
    }
}


/*void Shader::sceneInit(GLint g_program)
{
    GLint result;

    //create program object and attach shaders 
    //g_program = glCreateProgram();
    shaderAttachFromFile(g_program, GL_VERTEX_SHADER, "shader/vertex.glsl");
    shaderAttachFromFile(g_program, GL_FRAGMENT_SHADER, "shader/fragment.glsl");

    //link the program and make sure that there were no errors 
    glLinkProgram(g_program);
    glGetProgramiv(g_program, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) 
    {
        GLint length;
        char *log;

        //get the program info log 
        glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetProgramInfoLog(g_program, length, &result, log);

        //print an error message and the info log 
        fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
        free(log);

        //delete the program 
        glDeleteProgram(g_program);
        g_program = 0;
    }


    g_programCameraPositionLocation = glGetUniformLocation(g_program, "cameraPosition");
    g_programLightPositionLocation = glGetUniformLocation(g_program, "lightPosition");
    g_programLightColorLocation = glGetUniformLocation(g_program, "lightColor");

    g_lightColor[0] = 1.0f; g_lightColor[1] = 0.0f; g_lightColor[2] = 0.0f;
    g_lightColor[3] = 0.0f; g_lightColor[4] = 1.0f; g_lightColor[5] = 0.0f;
    g_lightColor[6] = 0.0f; g_lightColor[7] = 0.0f; g_lightColor[8] = 1.0f;
}*/


char* Shader::shaderLoadSource(const char *filePath)
{
	FILE *f = fopen(filePath, "rb");
    if(!f)
    {
        std::cout << "Error opening the shader source file!" << std::endl;
        exit(1);
    }
    fseek(f,0,SEEK_END);
    long fsize = ftell(f);
    fseek(f,0,SEEK_SET);

    char* source = new char[fsize+1];
    fread(source, fsize, 1, f);
    fclose(f);
    source[fsize] = 0;
}
