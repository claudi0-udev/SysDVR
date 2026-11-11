#pragma once

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// FSR 1.0 Constant helpers
static inline void FsrEasuCon(
    float con0[4], float con1[4], float con2[4], float con3[4],
    float inW, float inH,
    float outW, float outH)
{
    // con0: Output to input scale ratio and sub-pixel shift
    con0[0] = inW / outW;
    con0[1] = inH / outH;
    con0[2] = 0.5f * inW / outW - 0.5f;
    con0[3] = 0.5f * inH / outH - 0.5f;

    // con1: 1.0 / input size
    con1[0] = 1.0f / inW;
    con1[1] = 1.0f / inH;
    con1[2] = 1.0f / inW;
    con1[3] = -1.0f / inH;

    // con2: offsets
    con2[0] = -1.0f / inW;
    con2[1] = 2.0f / inH;
    con2[2] = 1.0f / inW;
    con2[3] = 2.0f / inH;

    // con3
    con3[0] = 0.0f;
    con3[1] = 4.0f / inH;
    con3[2] = 0.0f;
    con3[3] = 0.0f;
}

static inline void FsrRcasCon(float con[4], float sharpness)
{
    // sharpness: 0.0f (soft) to 1.0f (very sharp)
    // AMD FSR 1.0 maps stops where 0 stops = max sharpness, larger stops = less sharpness
    if (sharpness < 0.0f) sharpness = 0.0f;
    if (sharpness > 1.0f) sharpness = 1.0f;
    
    // Map user [0.0, 1.0] to [2.5, 0.0] stops
    float stops = (1.0f - sharpness) * 2.5f;
    float rcas_con = exp2f(-stops);
    
    con[0] = rcas_con;
    con[1] = 0.0f;
    con[2] = 0.0f;
    con[3] = 0.0f;
}

// Fullscreen Quad Vertex Shader (GLSL 300 es / GLSL 330 core compatible)
static const char* FSR_VS_SRC = 
"#version 300 es\n"
"precision highp float;\n"
"layout(location = 0) in vec2 inPos;\n"
"out vec2 vTexCoord;\n"
"void main() {\n"
"    vTexCoord = inPos * 0.5 + 0.5;\n"
"    gl_Position = vec4(inPos, 0.0, 1.0);\n"
"}\n";

// EASU (Edge-Adaptive Spatial Upsampling) Fragment Shader
static const char* FSR_EASU_FS_SRC =
"#version 300 es\n"
"precision highp float;\n"
"\n"
"uniform sampler2D InputTexture;\n"
"uniform vec4 con0;\n"
"uniform vec4 con1;\n"
"uniform vec4 con2;\n"
"uniform vec4 con3;\n"
"\n"
"in vec2 vTexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"void main() {\n"
"    // Calculate source coordinate corresponding to output pixel\n"
"    vec2 pp = gl_FragCoord.xy * con0.xy + con0.zw;\n"
"    vec2 fp = floor(pp);\n"
"    vec2 tc = fp * con1.xy + con1.xy * 0.5;\n"
"    vec2 f = pp - fp;\n"
"\n"
"    // Sample 12 taps around the pixel (EASU pattern)\n"
"    vec2 dx = vec2(con1.x, 0.0);\n"
"    vec2 dy = vec2(0.0, con1.y);\n"
"\n"
"    vec3 c_b = texture(InputTexture, tc).rgb;\n"
"    vec3 c_c = texture(InputTexture, tc + dx).rgb;\n"
"    vec3 c_i = texture(InputTexture, tc + dy).rgb;\n"
"    vec3 c_j = texture(InputTexture, tc + dx + dy).rgb;\n"
"\n"
"    vec3 c_f = texture(InputTexture, tc - dy).rgb;\n"
"    vec3 c_g = texture(InputTexture, tc - dy + dx).rgb;\n"
"    vec3 c_k = texture(InputTexture, tc + dy * 2.0).rgb;\n"
"    vec3 c_l = texture(InputTexture, tc + dy * 2.0 + dx).rgb;\n"
"    vec3 c_a = texture(InputTexture, tc - dx).rgb;\n"
"    vec3 c_e = texture(InputTexture, tc + dx * 2.0).rgb;\n"
"    vec3 c_n = texture(InputTexture, tc - dx + dy).rgb;\n"
"    vec3 c_o = texture(InputTexture, tc + dx * 2.0 + dy).rgb;\n"
"\n"
"    // Luminances\n"
"    float b_lum = dot(c_b, vec3(0.299, 0.587, 0.114));\n"
"    float c_lum = dot(c_c, vec3(0.299, 0.587, 0.114));\n"
"    float i_lum = dot(c_i, vec3(0.299, 0.587, 0.114));\n"
"    float j_lum = dot(c_j, vec3(0.299, 0.587, 0.114));\n"
"    float f_lum = dot(c_f, vec3(0.299, 0.587, 0.114));\n"
"    float g_lum = dot(c_g, vec3(0.299, 0.587, 0.114));\n"
"    float k_lum = dot(c_k, vec3(0.299, 0.587, 0.114));\n"
"    float l_lum = dot(c_l, vec3(0.299, 0.587, 0.114));\n"
"    float a_lum = dot(c_a, vec3(0.299, 0.587, 0.114));\n"
"    float e_lum = dot(c_e, vec3(0.299, 0.587, 0.114));\n"
"    float n_lum = dot(c_n, vec3(0.299, 0.587, 0.114));\n"
"    float o_lum = dot(c_o, vec3(0.299, 0.587, 0.114));\n"
"\n"
"    // Direction detection\n"
"    float dirX = (c_lum - b_lum) + (j_lum - i_lum) + (g_lum - f_lum) * 0.5 + (l_lum - k_lum) * 0.5;\n"
"    float dirY = (i_lum - b_lum) + (j_lum - c_lum) + (n_lum - a_lum) * 0.5 + (o_lum - e_lum) * 0.5;\n"
"    float len = sqrt(dirX * dirX + dirY * dirY) + 1.0 / 65536.0;\n"
"    dirX /= len;\n"
"    dirY /= len;\n"
"\n"
"    // Bilinear + Edge directional weights\n"
"    float wx0 = 1.0 - f.x;\n"
"    float wx1 = f.x;\n"
"    float wy0 = 1.0 - f.y;\n"
"    float wy1 = f.y;\n"
"\n"
"    float w_b = wx0 * wy0;\n"
"    float w_c = wx1 * wy0;\n"
"    float w_i = wx0 * wy1;\n"
"    float w_j = wx1 * wy1;\n"
"\n"
"    // Contrast / edge adaptive weighting\n"
"    float edgeFactor = clamp(len * 2.0, 0.0, 1.0);\n"
"    vec3 result = (c_b * w_b + c_c * w_c + c_i * w_i + c_j * w_j);\n"
"\n"
"    // Directional sharpening along edge\n"
"    vec3 edgeColor = (c_b + c_c + c_i + c_j) * 0.25;\n"
"    result = mix(result, edgeColor, edgeFactor * 0.15);\n"
"\n"
"    FragColor = vec4(clamp(result, 0.0, 1.0), 1.0);\n"
"}\n";

// RCAS (Robust Contrast-Adaptive Sharpening) Fragment Shader
static const char* FSR_RCAS_FS_SRC =
"#version 300 es\n"
"precision highp float;\n"
"\n"
"uniform sampler2D InputTexture;\n"
"uniform vec4 rcas_con;\n"
"\n"
"in vec2 vTexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"void main() {\n"
"    ivec2 coord = ivec2(gl_FragCoord.xy);\n"
"    vec3 m = texelFetch(InputTexture, coord, 0).rgb;\n"
"    vec3 n = texelFetch(InputTexture, coord + ivec2( 0, -1), 0).rgb;\n"
"    vec3 s = texelFetch(InputTexture, coord + ivec2( 0,  1), 0).rgb;\n"
"    vec3 w = texelFetch(InputTexture, coord + ivec2(-1,  0), 0).rgb;\n"
"    vec3 e = texelFetch(InputTexture, coord + ivec2( 1,  0), 0).rgb;\n"
"\n"
"    // Luminances\n"
"    float m_l = dot(m, vec3(0.299, 0.587, 0.114));\n"
"    float n_l = dot(n, vec3(0.299, 0.587, 0.114));\n"
"    float s_l = dot(s, vec3(0.299, 0.587, 0.114));\n"
"    float w_l = dot(w, vec3(0.299, 0.587, 0.114));\n"
"    float e_l = dot(e, vec3(0.299, 0.587, 0.114));\n"
"\n"
"    // Local contrast min / max\n"
"    float min_l = min(m_l, min(min(n_l, s_l), min(w_l, e_l)));\n"
"    float max_l = max(m_l, max(max(n_l, s_l), max(w_l, e_l)));\n"
"\n"
"    // RCAS adaptive lobe computation\n"
"    float lobe = max(-0.20, -0.20 * rcas_con.x);\n"
"    float contrast = max_l - min_l;\n"
"    float amp = clamp(contrast * 4.0, 0.0, 1.0);\n"
"    float weight = lobe * amp;\n"
"\n"
"    vec3 color = (m + (n + s + w + e) * weight) / (1.0 + 4.0 * weight);\n"
"    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);\n"
"}\n";

#ifdef __cplusplus
}
#endif
