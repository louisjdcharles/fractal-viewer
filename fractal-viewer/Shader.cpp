#include "Shader.h"

GLint CompileShader(const std::string& path, GLenum type)
{
    GLuint id = glCreateShader(type);
    std::string contents = Files::ReadFile(path).c_str();
    const char* src_as_cstring = contents.c_str();
    glShaderSource(id, 1, &src_as_cstring, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

        char* msg = (char*)alloca(len * sizeof(char));

        glGetShaderInfoLog(id, len, &len, msg);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "\n";
        std::cout << msg << "\n";

        return 0;
    }

    return id;
}

GLuint CreateComputeProgram(const std::string& path) {
    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    std::string compute_shader_src = Files::ReadFile(path);
    const char* compute_src_cstr = compute_shader_src.c_str();

    (glShaderSource(compute_shader, 1, &compute_src_cstr, NULL));
    (glCompileShader(compute_shader));

    GLint result;
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int len;
        glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &len);

        char* msg = (char*)alloca(len * sizeof(char));

        glGetShaderInfoLog(compute_shader, len, &len, msg);

        std::cout << "Failed to compile compute shader:" << std::endl;
        std::cout << msg << "\n";

        assert(false);
    }

    GLuint compute_program = glCreateProgram();
    (glAttachShader(compute_program, compute_shader));
    (glLinkProgram(compute_program));

    glGetProgramiv(compute_program, GL_LINK_STATUS, &result);

    if (result == GL_FALSE)
    {
        int len;
        glGetProgramiv(compute_program, GL_INFO_LOG_LENGTH, &len);

        char* msg = (char*)alloca(len * sizeof(char));

        glGetProgramInfoLog(compute_program, len, &len, msg);

        std::cout << "Failed to link compute shader:" << std::endl;
        std::cout << msg << "\n";

        assert(false);
    }

    glDeleteShader(compute_shader);

    return compute_program;
}
