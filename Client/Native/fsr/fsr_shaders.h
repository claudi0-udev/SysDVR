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
    con3[2] = outH;
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

// AMD FidelityFX FSR 1.0 - EASU (Edge-Adaptive Spatial Upsampling) Fragment Shader
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
"void FsrEasuTapF(\n"
"    inout vec3 aC,\n"
"    inout float aW,\n"
"    vec2 off,\n"
"    vec2 dir,\n"
"    vec2 len,\n"
"    float lob,\n"
"    float clp,\n"
"    vec3 c\n"
") {\n"
"    vec2 v = vec2(dot(off, dir), dot(off, vec2(-dir.y, dir.x)));\n"
"    v *= len;\n"
"    float d2 = min(dot(v, v), clp);\n"
"    float wB = 0.4 * d2 - 1.0;\n"
"    float wA = lob * d2 - 1.0;\n"
"    wB *= wB;\n"
"    wA *= wA;\n"
"    wB = 1.5625 * wB - 0.5625;\n"
"    float w = wB * wA;\n"
"    aC += c * w;\n"
"    aW += w;\n"
"}\n"
"\n"
"void FsrEasuSetF(\n"
"    inout vec2 dir,\n"
"    inout float len,\n"
"    float w,\n"
"    float lA, float lB, float lC, float lD, float lE\n"
") {\n"
"    float lenX = max(abs(lD - lC), abs(lC - lB));\n"
"    float dirX = lD - lB;\n"
"    dir.x += dirX * w;\n"
"    lenX = clamp(abs(dirX) / (lenX + 0.00001), 0.0, 1.0);\n"
"    lenX *= lenX;\n"
"    len += lenX * w;\n"
"\n"
"    float lenY = max(abs(lE - lC), abs(lC - lA));\n"
"    float dirY = lE - lA;\n"
"    dir.y += dirY * w;\n"
"    lenY = clamp(abs(dirY) / (lenY + 0.00001), 0.0, 1.0);\n"
"    lenY *= lenY;\n"
"    len += lenY * w;\n"
"}\n"
"\n"
"void FsrEasuF(\n"
"    out vec3 pix,\n"
"    vec2 ip,\n"
"    vec4 c0, vec4 c1, vec4 c2, vec4 c3\n"
") {\n"
"    vec2 pp = ip * c0.xy + c0.zw;\n"
"    vec2 fp = floor(pp);\n"
"    pp -= fp;\n"
"\n"
"    vec2 p0 = fp * c1.xy + c1.zw;\n"
"    vec2 p1 = p0 + c2.xy;\n"
"    vec2 p2 = p0 + c2.zw;\n"
"    vec2 p3 = p0 + c3.xy;\n"
"\n"
"    vec4 off = vec4(-0.5, 0.5, -0.5, 0.5) * c1.xxyy;\n"
"\n"
"    vec3 bC = texture(InputTexture, p0 + off.xw).rgb; float bL = bC.g + 0.5 * (bC.r + bC.b);\n"
"    vec3 cC = texture(InputTexture, p0 + off.yw).rgb; float cL = cC.g + 0.5 * (cC.r + cC.b);\n"
"    vec3 iC = texture(InputTexture, p1 + off.xw).rgb; float iL = iC.g + 0.5 * (iC.r + iC.b);\n"
"    vec3 jC = texture(InputTexture, p1 + off.yw).rgb; float jL = jC.g + 0.5 * (jC.r + jC.b);\n"
"    vec3 fC = texture(InputTexture, p1 + off.yz).rgb; float fL = fC.g + 0.5 * (fC.r + fC.b);\n"
"    vec3 eC = texture(InputTexture, p1 + off.xz).rgb; float eL = eC.g + 0.5 * (eC.r + eC.b);\n"
"    vec3 kC = texture(InputTexture, p2 + off.xw).rgb; float kL = kC.g + 0.5 * (kC.r + kC.b);\n"
"    vec3 lC = texture(InputTexture, p2 + off.yw).rgb; float lL = lC.g + 0.5 * (lC.r + lC.b);\n"
"    vec3 hC = texture(InputTexture, p2 + off.yz).rgb; float hL = hC.g + 0.5 * (hC.r + hC.b);\n"
"    vec3 gC = texture(InputTexture, p2 + off.xz).rgb; float gL = gC.g + 0.5 * (gC.r + gC.b);\n"
"    vec3 oC = texture(InputTexture, p3 + off.yz).rgb; float oL = oC.g + 0.5 * (oC.r + oC.b);\n"
"    vec3 nC = texture(InputTexture, p3 + off.xz).rgb; float nL = nC.g + 0.5 * (nC.r + nC.b);\n"
"\n"
"    vec2 dir = vec2(0.0);\n"
"    float len = 0.0;\n"
"\n"
"    FsrEasuSetF(dir, len, (1.0 - pp.x) * (1.0 - pp.y), bL, eL, fL, gL, jL);\n"
"    FsrEasuSetF(dir, len,        pp.x  * (1.0 - pp.y), cL, fL, gL, hL, kL);\n"
"    FsrEasuSetF(dir, len, (1.0 - pp.x) *        pp.y , fL, iL, jL, kL, nL);\n"
"    FsrEasuSetF(dir, len,        pp.x  *        pp.y , gL, jL, kL, lL, oL);\n"
"\n"
"    vec2 dir2 = dir * dir;\n"
"    float dirR = dir2.x + dir2.y;\n"
"    bool zro = dirR < (1.0 / 32768.0);\n"
"    dirR = inversesqrt(dirR);\n"
"    dirR = zro ? 1.0 : dirR;\n"
"    dir.x = zro ? 1.0 : dir.x;\n"
"    dir *= vec2(dirR);\n"
"\n"
"    len = len * 0.5;\n"
"    len *= len;\n"
"\n"
"    float stretch = dot(dir, dir) / (max(abs(dir.x), abs(dir.y)) + 0.00001);\n"
"    vec2 len2 = vec2(1.0 + (stretch - 1.0) * len, 1.0 - 0.5 * len);\n"
"    float lob = 0.5 - 0.29 * len;\n"
"    float clp = 1.0 / lob;\n"
"\n"
"    vec3 min4 = min(min(fC, gC), min(jC, kC));\n"
"    vec3 max4 = max(max(fC, gC), max(jC, kC));\n"
"\n"
"    vec3 aC = vec3(0.0);\n"
"    float aW = 0.0;\n"
"    FsrEasuTapF(aC, aW, vec2( 0.0, -1.0) - pp, dir, len2, lob, clp, bC);\n"
"    FsrEasuTapF(aC, aW, vec2( 1.0, -1.0) - pp, dir, len2, lob, clp, cC);\n"
"    FsrEasuTapF(aC, aW, vec2(-1.0,  1.0) - pp, dir, len2, lob, clp, iC);\n"
"    FsrEasuTapF(aC, aW, vec2( 0.0,  1.0) - pp, dir, len2, lob, clp, jC);\n"
"    FsrEasuTapF(aC, aW, vec2( 0.0,  0.0) - pp, dir, len2, lob, clp, fC);\n"
"    FsrEasuTapF(aC, aW, vec2(-1.0,  0.0) - pp, dir, len2, lob, clp, eC);\n"
"    FsrEasuTapF(aC, aW, vec2( 1.0,  1.0) - pp, dir, len2, lob, clp, kC);\n"
"    FsrEasuTapF(aC, aW, vec2( 2.0,  1.0) - pp, dir, len2, lob, clp, lC);\n"
"    FsrEasuTapF(aC, aW, vec2( 2.0,  0.0) - pp, dir, len2, lob, clp, hC);\n"
"    FsrEasuTapF(aC, aW, vec2( 1.0,  0.0) - pp, dir, len2, lob, clp, gC);\n"
"    FsrEasuTapF(aC, aW, vec2( 1.0,  2.0) - pp, dir, len2, lob, clp, oC);\n"
"    FsrEasuTapF(aC, aW, vec2( 0.0,  2.0) - pp, dir, len2, lob, clp, nC);\n"
"\n"
"    pix = min(max4, max(min4, aC / aW));\n"
"}\n"
"\n"
"void main() {\n"
"    float flipped_y = con3.z - gl_FragCoord.y;\n"
"    vec2 fragCoord = vec2(gl_FragCoord.x, flipped_y);\n"
"    vec3 c;\n"
"    FsrEasuF(c, fragCoord, con0, con1, con2, con3);\n"
"    FragColor = vec4(clamp(c, 0.0, 1.0), 1.0);\n"
"}\n";

// AMD FidelityFX FSR 1.0 - RCAS (Robust Contrast-Adaptive Sharpening) Fragment Shader
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
"#define FSR_RCAS_LIMIT (0.25 - (1.0 / 16.0))\n"
"\n"
"void main() {\n"
"    ivec2 size = textureSize(InputTexture, 0);\n"
"    ivec2 coord = clamp(ivec2(vTexCoord * vec2(size)), ivec2(0), size - 1);\n"
"\n"
"    vec3 b = texelFetch(InputTexture, clamp(coord + ivec2( 0, -1), ivec2(0), size - 1), 0).rgb;\n"
"    vec3 d = texelFetch(InputTexture, clamp(coord + ivec2(-1,  0), ivec2(0), size - 1), 0).rgb;\n"
"    vec3 e = texelFetch(InputTexture, coord, 0).rgb;\n"
"    vec3 f = texelFetch(InputTexture, clamp(coord + ivec2( 1,  0), ivec2(0), size - 1), 0).rgb;\n"
"    vec3 h = texelFetch(InputTexture, clamp(coord + ivec2( 0,  1), ivec2(0), size - 1), 0).rgb;\n"
"\n"
"    // Luma times 2\n"
"    float bL = b.g + 0.5 * (b.b + b.r);\n"
"    float dL = d.g + 0.5 * (d.b + d.r);\n"
"    float eL = e.g + 0.5 * (e.b + e.r);\n"
"    float fL = f.g + 0.5 * (f.b + f.r);\n"
"    float hL = h.g + 0.5 * (h.b + h.r);\n"
"\n"
"    // Noise detection\n"
"    float nz = 0.25 * (bL + dL + fL + hL) - eL;\n"
"    float range = max(max(bL, dL), max(eL, max(fL, hL))) - min(min(bL, dL), min(eL, min(fL, hL)));\n"
"    nz = clamp(abs(nz) / (range + 0.0001), 0.0, 1.0);\n"
"    nz = 1.0 - 0.5 * nz;\n"
"\n"
"    // Min and max of ring\n"
"    vec3 mn4 = min(b, min(f, min(h, d)));\n"
"    vec3 mx4 = max(b, max(f, max(h, d)));\n"
"\n"
"    // Limiters\n"
"    vec2 peakC = vec2(1.0, -4.0);\n"
"    vec3 hitMin = mn4 / (4.0 * mx4 + 0.0001);\n"
"    vec3 hitMax = (peakC.x - mx4) / (4.0 * mn4 + peakC.y);\n"
"    vec3 lobeRGB = max(-hitMin, hitMax);\n"
"    float lobe = max(\n"
"        -FSR_RCAS_LIMIT,\n"
"        min(max(lobeRGB.r, max(lobeRGB.g, lobeRGB.b)), 0.0)\n"
"    ) * rcas_con.x;\n"
"\n"
"    lobe *= nz;\n"
"\n"
"    // Resolve\n"
"    vec3 color = (lobe * (b + d + h + f) + e) / (4.0 * lobe + 1.0);\n"
"    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);\n"
"}\n";

// Linear Downsample / Passthrough Blit Fragment Shader (for Super Sampling)
static const char* FSR_BLIT_FS_SRC =
"#version 300 es\n"
"precision highp float;\n"
"\n"
"uniform sampler2D BlitTexture;\n"
"in vec2 vTexCoord;\n"
"out vec4 FragColor;\n"
"\n"
"void main() {\n"
"    FragColor = texture(BlitTexture, vTexCoord);\n"
"}\n";

#ifdef __cplusplus
}
#endif
