#include "fsr_pipeline.h"
#include "fsr_shaders.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// OpenGL definitions & function pointer prototypes
#ifndef GL_FLOAT
#define GL_FLOAT                          0x1406
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_LINEAR                         0x2601
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_RGBA                           0x1908
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_FRAMEBUFFER                    0x8D40
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_VIEWPORT                       0x0BA2
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_TEXTURE_BINDING_2D             0x8069
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_BLEND                          0x0BE2
#define GL_CULL_FACE                      0x0B44
#define GL_DEPTH_TEST                     0x0B71
#define GL_SCISSOR_TEST                   0x0C11

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef char GLchar;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
#endif

// OpenGL Function Pointers
typedef GLuint (*PFN_glCreateShader)(GLenum type);
typedef void (*PFN_glShaderSource)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (*PFN_glCompileShader)(GLuint shader);
typedef void (*PFN_glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
typedef void (*PFN_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (*PFN_glDeleteShader)(GLuint shader);
typedef GLuint (*PFN_glCreateProgram)(void);
typedef void (*PFN_glAttachShader)(GLuint program, GLuint shader);
typedef void (*PFN_glLinkProgram)(GLuint program);
typedef void (*PFN_glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
typedef void (*PFN_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (*PFN_glDeleteProgram)(GLuint program);
typedef void (*PFN_glUseProgram)(GLuint program);
typedef GLint (*PFN_glGetUniformLocation)(GLuint program, const GLchar* name);
typedef void (*PFN_glUniform1i)(GLint location, GLint v0);
typedef void (*PFN_glUniform4fv)(GLint location, GLsizei count, const GLfloat* value);
typedef void (*PFN_glGenBuffers)(GLsizei n, GLuint* buffers);
typedef void (*PFN_glBindBuffer)(GLenum target, GLuint buffer);
typedef void (*PFN_glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (*PFN_glDeleteBuffers)(GLsizei n, const GLuint* buffers);
typedef void (*PFN_glEnableVertexAttribArray)(GLuint index);
typedef void (*PFN_glDisableVertexAttribArray)(GLuint index);
typedef void (*PFN_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (*PFN_glGenFramebuffers)(GLsizei n, GLuint* framebuffers);
typedef void (*PFN_glBindFramebuffer)(GLenum target, GLuint framebuffer);
typedef void (*PFN_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (*PFN_glCheckFramebufferStatus)(GLenum target);
typedef void (*PFN_glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
typedef void (*PFN_glGenTextures)(GLsizei n, GLuint* textures);
typedef void (*PFN_glBindTexture)(GLenum target, GLuint texture);
typedef void (*PFN_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void (*PFN_glTexParameteri)(GLenum target, GLenum pname, GLint param);
typedef void (*PFN_glDeleteTextures)(GLsizei n, const GLuint* textures);
typedef void (*PFN_glActiveTexture)(GLenum texture);
typedef void (*PFN_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (*PFN_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
typedef void (*PFN_glGetIntegerv)(GLenum pname, GLint* data);
typedef GLboolean (*PFN_glIsEnabled)(GLenum cap);
typedef void (*PFN_glEnable)(GLenum cap);
typedef void (*PFN_glDisable)(GLenum cap);

struct GLContext {
    PFN_glCreateShader glCreateShader;
    PFN_glShaderSource glShaderSource;
    PFN_glCompileShader glCompileShader;
    PFN_glGetShaderiv glGetShaderiv;
    PFN_glGetShaderInfoLog glGetShaderInfoLog;
    PFN_glDeleteShader glDeleteShader;
    PFN_glCreateProgram glCreateProgram;
    PFN_glAttachShader glAttachShader;
    PFN_glLinkProgram glLinkProgram;
    PFN_glGetProgramiv glGetProgramiv;
    PFN_glGetProgramInfoLog glGetProgramInfoLog;
    PFN_glDeleteProgram glDeleteProgram;
    PFN_glUseProgram glUseProgram;
    PFN_glGetUniformLocation glGetUniformLocation;
    PFN_glUniform1i glUniform1i;
    PFN_glUniform4fv glUniform4fv;
    PFN_glGenBuffers glGenBuffers;
    PFN_glBindBuffer glBindBuffer;
    PFN_glBufferData glBufferData;
    PFN_glDeleteBuffers glDeleteBuffers;
    PFN_glEnableVertexAttribArray glEnableVertexAttribArray;
    PFN_glDisableVertexAttribArray glDisableVertexAttribArray;
    PFN_glVertexAttribPointer glVertexAttribPointer;
    PFN_glGenFramebuffers glGenFramebuffers;
    PFN_glBindFramebuffer glBindFramebuffer;
    PFN_glFramebufferTexture2D glFramebufferTexture2D;
    PFN_glCheckFramebufferStatus glCheckFramebufferStatus;
    PFN_glDeleteFramebuffers glDeleteFramebuffers;
    PFN_glGenTextures glGenTextures;
    PFN_glBindTexture glBindTexture;
    PFN_glTexImage2D glTexImage2D;
    PFN_glTexParameteri glTexParameteri;
    PFN_glDeleteTextures glDeleteTextures;
    PFN_glActiveTexture glActiveTexture;
    PFN_glViewport glViewport;
    PFN_glDrawArrays glDrawArrays;
    PFN_glGetIntegerv glGetIntegerv;
    PFN_glIsEnabled glIsEnabled;
    PFN_glEnable glEnable;
    PFN_glDisable glDisable;
    bool loaded;
};

static GLContext gl = {0};

static bool load_gl_funcs() {
    if (gl.loaded) return true;

    #define LOAD_PROC(name) \
        gl.name = (PFN_##name)SDL_GL_GetProcAddress(#name); \
        if (!gl.name) { \
            fprintf(stderr, "[FSR] Failed to load GL procedure: %s\n", #name); \
            return false; \
        }

    LOAD_PROC(glCreateShader);
    LOAD_PROC(glShaderSource);
    LOAD_PROC(glCompileShader);
    LOAD_PROC(glGetShaderiv);
    LOAD_PROC(glGetShaderInfoLog);
    LOAD_PROC(glDeleteShader);
    LOAD_PROC(glCreateProgram);
    LOAD_PROC(glAttachShader);
    LOAD_PROC(glLinkProgram);
    LOAD_PROC(glGetProgramiv);
    LOAD_PROC(glGetProgramInfoLog);
    LOAD_PROC(glDeleteProgram);
    LOAD_PROC(glUseProgram);
    LOAD_PROC(glGetUniformLocation);
    LOAD_PROC(glUniform1i);
    LOAD_PROC(glUniform4fv);
    LOAD_PROC(glGenBuffers);
    LOAD_PROC(glBindBuffer);
    LOAD_PROC(glBufferData);
    LOAD_PROC(glDeleteBuffers);
    LOAD_PROC(glEnableVertexAttribArray);
    LOAD_PROC(glDisableVertexAttribArray);
    LOAD_PROC(glVertexAttribPointer);
    LOAD_PROC(glGenFramebuffers);
    LOAD_PROC(glBindFramebuffer);
    LOAD_PROC(glFramebufferTexture2D);
    LOAD_PROC(glCheckFramebufferStatus);
    LOAD_PROC(glDeleteFramebuffers);
    LOAD_PROC(glGenTextures);
    LOAD_PROC(glBindTexture);
    LOAD_PROC(glTexImage2D);
    LOAD_PROC(glTexParameteri);
    LOAD_PROC(glDeleteTextures);
    LOAD_PROC(glActiveTexture);
    LOAD_PROC(glViewport);
    LOAD_PROC(glDrawArrays);
    LOAD_PROC(glGetIntegerv);
    LOAD_PROC(glIsEnabled);
    LOAD_PROC(glEnable);
    LOAD_PROC(glDisable);
    #undef LOAD_PROC

    gl.loaded = true;
    return true;
}

// Pipeline State
static struct {
    bool initialized;
    int src_w;
    int src_h;
    int fbo_w;
    int fbo_h;

    // Fullscreen quad
    GLuint quad_vbo;

    // EASU program & uniforms
    GLuint prog_easu;
    GLint loc_easu_tex;
    GLint loc_easu_con0;
    GLint loc_easu_con1;
    GLint loc_easu_con2;
    GLint loc_easu_con3;

    // RCAS program & uniforms
    GLuint prog_rcas;
    GLint loc_rcas_tex;
    GLint loc_rcas_con;

    // Intermediate EASU FBO & Texture
    GLuint fbo_easu;
    GLuint tex_easu;

    // SDL 720p RGBA Render Target Texture
    SDL_Texture* sdl_rgba_src;
} fsr = {0};

static GLuint compile_shader(GLenum type, const char* src) {
    GLuint shader = gl.glCreateShader(type);
    gl.glShaderSource(shader, 1, &src, NULL);
    gl.glCompileShader(shader);

    GLint status = 0;
    gl.glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        gl.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen + 1);
        if (log) {
            gl.glGetShaderInfoLog(shader, logLen, NULL, log);
            fprintf(stderr, "[FSR] Shader compilation failed:\n%s\n", log);
            free(log);
        }
        gl.glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint link_program(GLuint vs, GLuint fs) {
    GLuint prog = gl.glCreateProgram();
    gl.glAttachShader(prog, vs);
    gl.glAttachShader(prog, fs);
    gl.glLinkProgram(prog);

    GLint status = 0;
    gl.glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (!status) {
        GLint logLen = 0;
        gl.glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen + 1);
        if (log) {
            gl.glGetProgramInfoLog(prog, logLen, NULL, log);
            fprintf(stderr, "[FSR] Program link failed:\n%s\n", log);
            free(log);
        }
        gl.glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

static bool resize_easu_fbo(int w, int h) {
    if (fsr.fbo_w == w && fsr.fbo_h == h && fsr.fbo_easu != 0)
        return true;

    if (fsr.tex_easu != 0) {
        gl.glDeleteTextures(1, &fsr.tex_easu);
        fsr.tex_easu = 0;
    }
    if (fsr.fbo_easu != 0) {
        gl.glDeleteFramebuffers(1, &fsr.fbo_easu);
        fsr.fbo_easu = 0;
    }

    gl.glGenTextures(1, &fsr.tex_easu);
    gl.glBindTexture(GL_TEXTURE_2D, fsr.tex_easu);
    gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    gl.glGenFramebuffers(1, &fsr.fbo_easu);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fsr.fbo_easu);
    gl.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fsr.tex_easu, 0);

    GLenum status = gl.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "[FSR] Failed to create complete FBO for EASU (status 0x%X)\n", status);
        return false;
    }

    fsr.fbo_w = w;
    fsr.fbo_h = h;
    return true;
}

bool fsr_is_supported() {
    return load_gl_funcs();
}

bool fsr_init(void* sdlRenderer, int srcW, int srcH) {
    if (fsr.initialized && fsr.src_w == srcW && fsr.src_h == srcH)
        return true;

    if (!load_gl_funcs())
        return false;

    fsr_destroy();

    // Compile EASU Program
    GLuint vs = compile_shader(GL_VERTEX_SHADER, FSR_VS_SRC);
    if (!vs) return false;

    GLuint fs_easu = compile_shader(GL_FRAGMENT_SHADER, FSR_EASU_FS_SRC);
    if (!fs_easu) {
        gl.glDeleteShader(vs);
        return false;
    }

    fsr.prog_easu = link_program(vs, fs_easu);
    gl.glDeleteShader(fs_easu);
    if (!fsr.prog_easu) {
        gl.glDeleteShader(vs);
        return false;
    }

    fsr.loc_easu_tex  = gl.glGetUniformLocation(fsr.prog_easu, "InputTexture");
    fsr.loc_easu_con0 = gl.glGetUniformLocation(fsr.prog_easu, "con0");
    fsr.loc_easu_con1 = gl.glGetUniformLocation(fsr.prog_easu, "con1");
    fsr.loc_easu_con2 = gl.glGetUniformLocation(fsr.prog_easu, "con2");
    fsr.loc_easu_con3 = gl.glGetUniformLocation(fsr.prog_easu, "con3");

    // Compile RCAS Program
    GLuint fs_rcas = compile_shader(GL_FRAGMENT_SHADER, FSR_RCAS_FS_SRC);
    if (!fs_rcas) {
        gl.glDeleteShader(vs);
        gl.glDeleteProgram(fsr.prog_easu);
        return false;
    }

    fsr.prog_rcas = link_program(vs, fs_rcas);
    gl.glDeleteShader(fs_rcas);
    gl.glDeleteShader(vs);
    if (!fsr.prog_rcas) {
        gl.glDeleteProgram(fsr.prog_easu);
        return false;
    }

    fsr.loc_rcas_tex = gl.glGetUniformLocation(fsr.prog_rcas, "InputTexture");
    fsr.loc_rcas_con = gl.glGetUniformLocation(fsr.prog_rcas, "rcas_con");

    // Fullscreen quad VBO (NDC coordinates: -1 to 1)
    static const float quad_verts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    gl.glGenBuffers(1, &fsr.quad_vbo);
    gl.glBindBuffer(GL_ARRAY_BUFFER, fsr.quad_vbo);
    gl.glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
    gl.glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create SDL RGBA Render Target for YUV -> RGBA conversion
    SDL_Renderer* renderer = (SDL_Renderer*)sdlRenderer;
    fsr.sdl_rgba_src = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        srcW, srcH
    );

    if (!fsr.sdl_rgba_src) {
        fprintf(stderr, "[FSR] Failed to create SDL RGBA source texture: %s\n", SDL_GetError());
        fsr_destroy();
        return false;
    }

    fsr.src_w = srcW;
    fsr.src_h = srcH;
    fsr.initialized = true;

    printf("[FSR] FidelityFX FSR 1.0 Pipeline initialized for %dx%d source stream\n", srcW, srcH);
    return true;
}

bool fsr_render(
    void* sdlRenderer,
    void* sdlTexture,
    int srcW, int srcH,
    int dstX, int dstY, int dstW, int dstH,
    float sharpness
) {
    if (!fsr.initialized || fsr.src_w != srcW || fsr.src_h != srcH) {
        if (!fsr_init(sdlRenderer, srcW, srcH))
            return false;
    }

    if (!resize_easu_fbo(dstW, dstH))
        return false;

    SDL_Renderer* renderer = (SDL_Renderer*)sdlRenderer;
    SDL_Texture* yuvTex = (SDL_Texture*)sdlTexture;

    // Step 1: Hardware/GPU YUV -> RGBA conversion via SDL RenderCopy
    SDL_SetRenderTarget(renderer, fsr.sdl_rgba_src);
    SDL_RenderCopy(renderer, yuvTex, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);

    // Save OpenGL states to restore later
    GLint prev_fbo = 0;
    gl.glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    GLint prev_viewport[4] = {0};
    gl.glGetIntegerv(GL_VIEWPORT, prev_viewport);
    GLint prev_program = 0;
    gl.glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);
    GLint prev_active_tex = 0;
    gl.glGetIntegerv(GL_ACTIVE_TEXTURE, &prev_active_tex);
    GLint prev_tex = 0;
    gl.glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_tex);
    GLint prev_vbo = 0;
    gl.glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vbo);

    GLboolean prev_blend = gl.glIsEnabled(GL_BLEND);
    GLboolean prev_cull = gl.glIsEnabled(GL_CULL_FACE);
    GLboolean prev_depth = gl.glIsEnabled(GL_DEPTH_TEST);
    GLboolean prev_scissor = gl.glIsEnabled(GL_SCISSOR_TEST);

    gl.glDisable(GL_BLEND);
    gl.glDisable(GL_CULL_FACE);
    gl.glDisable(GL_DEPTH_TEST);
    gl.glDisable(GL_SCISSOR_TEST);

    // Prepare Vertex Buffer
    gl.glBindBuffer(GL_ARRAY_BUFFER, fsr.quad_vbo);
    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // -------------------------------------------------------------
    // PASS 1: EASU (Upscaling from 720p RGBA to Target Resolution FBO)
    // -------------------------------------------------------------
    gl.glBindFramebuffer(GL_FRAMEBUFFER, fsr.fbo_easu);
    gl.glViewport(0, 0, dstW, dstH);

    gl.glUseProgram(fsr.prog_easu);

    float con0[4], con1[4], con2[4], con3[4];
    FsrEasuCon(con0, con1, con2, con3, (float)srcW, (float)srcH, (float)dstW, (float)dstH);
    gl.glUniform4fv(fsr.loc_easu_con0, 1, con0);
    gl.glUniform4fv(fsr.loc_easu_con1, 1, con1);
    gl.glUniform4fv(fsr.loc_easu_con2, 1, con2);
    gl.glUniform4fv(fsr.loc_easu_con3, 1, con3);

    gl.glActiveTexture(GL_TEXTURE0);
    float tw = 1.0f, th = 1.0f;
    SDL_GL_BindTexture(fsr.sdl_rgba_src, &tw, &th);
    gl.glUniform1i(fsr.loc_easu_tex, 0);

    gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    SDL_GL_UnbindTexture(fsr.sdl_rgba_src);

    // -------------------------------------------------------------
    // PASS 2: RCAS (Sharpening to Screen Viewport)
    // -------------------------------------------------------------
    gl.glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    
    // In OpenGL, viewport Y starts from bottom-left
    int win_w = 0, win_h = 0;
    SDL_GetRendererOutputSize(renderer, &win_w, &win_h);
    int gl_y = win_h - (dstY + dstH);
    gl.glViewport(dstX, gl_y, dstW, dstH);

    gl.glUseProgram(fsr.prog_rcas);

    float rcas_con[4];
    FsrRcasCon(rcas_con, sharpness);
    gl.glUniform4fv(fsr.loc_rcas_con, 1, rcas_con);

    gl.glActiveTexture(GL_TEXTURE0);
    gl.glBindTexture(GL_TEXTURE_2D, fsr.tex_easu);
    gl.glUniform1i(fsr.loc_rcas_tex, 0);

    gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // -------------------------------------------------------------
    // Restore OpenGL State
    // -------------------------------------------------------------
    gl.glDisableVertexAttribArray(0);
    gl.glBindBuffer(GL_ARRAY_BUFFER, prev_vbo);
    gl.glUseProgram(prev_program);

    gl.glActiveTexture(prev_active_tex);
    gl.glBindTexture(GL_TEXTURE_2D, prev_tex);

    gl.glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    gl.glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);

    if (prev_blend) gl.glEnable(GL_BLEND);
    if (prev_cull) gl.glEnable(GL_CULL_FACE);
    if (prev_depth) gl.glEnable(GL_DEPTH_TEST);
    if (prev_scissor) gl.glEnable(GL_SCISSOR_TEST);

    return true;
}

void fsr_destroy() {
    if (!fsr.initialized) return;

    if (fsr.sdl_rgba_src) {
        SDL_DestroyTexture(fsr.sdl_rgba_src);
        fsr.sdl_rgba_src = NULL;
    }

    if (gl.loaded) {
        if (fsr.tex_easu) {
            gl.glDeleteTextures(1, &fsr.tex_easu);
            fsr.tex_easu = 0;
        }
        if (fsr.fbo_easu) {
            gl.glDeleteFramebuffers(1, &fsr.fbo_easu);
            fsr.fbo_easu = 0;
        }
        if (fsr.quad_vbo) {
            gl.glDeleteBuffers(1, &fsr.quad_vbo);
            fsr.quad_vbo = 0;
        }
        if (fsr.prog_easu) {
            gl.glDeleteProgram(fsr.prog_easu);
            fsr.prog_easu = 0;
        }
        if (fsr.prog_rcas) {
            gl.glDeleteProgram(fsr.prog_rcas);
            fsr.prog_rcas = 0;
        }
    }

    memset(&fsr, 0, sizeof(fsr));
}
