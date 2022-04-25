#pragma once

namespace Shaders::Tempo {

static const char* vertex_shader_source = R"(
#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;

uniform mat4 ProjMtx;

void main() {
  gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)";

static const char* fragment_shader_source = R"(
#version 330 core

uniform vec4 BBox;
uniform float Time;

layout (location = 0) out vec4 o_color;

vec2 getResolution() {
  return vec2(BBox.z - BBox.x, BBox.w - BBox.y);
}

vec2 getFragCoord() {
  return gl_FragCoord.xy - BBox.xy;
}

const float kPi = 3.1415926535;
const float kTau = 2.0 * kPi;

vec3 hue2rgb(float hue) {
  hue = fract(hue);
  float r = abs(hue * 6.0 - 3.0) - 1.0;
  float g = 2.0 - abs(hue * 6.0 - 2.0);
  float b = 2.0 - abs(hue * 6.0 - 4.0);
  return clamp(vec3(r, g, b), 0.0, 1.0);
}

float ring(float begin, float end, float smoothing, float value) {
  return smoothstep(begin, begin + smoothing, value) - smoothstep(end - smoothing, end, value);
}

void main() {
  vec2 resolution = getResolution();
  vec2 fragCoord = getFragCoord();

  vec2 p = fragCoord.xy / resolution.xy - 0.5;
  p.x *= resolution.x / resolution.y;

  float angle = atan(p.y, p.x) / kTau;
  float len = length(p);

  float r = ring(0.35, 0.42, 0.03, len);

  vec3 background_color = vec3(0.06);

  vec3 c = 2.0 * r * hue2rgb(angle + Time / 6.0);
  c = mix(background_color, c, smoothstep(0.0, 0.3, r));

  o_color = vec4(c, 1.0);
}
)";

}

namespace Shaders::Subdivision {

static const char* vertex_shader_source = R"(
#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;

uniform mat4 ProjMtx;

void main() {
  gl_Position = ProjMtx * vec4(Position.xy,0,1);
}
)";

static const char* fragment_shader_source = R"(
#version 330 core

uniform vec4 BBox;
uniform float Time;
uniform int subdiv;

layout (location = 0) out vec4 o_color;

vec2 getResolution() {
  return vec2(BBox.z - BBox.x, BBox.w - BBox.y);
}

vec2 getFragCoord() {
  return gl_FragCoord.xy - BBox.xy;
}

const float kPi = 3.1415926535;
const float kTau = 2.0 * kPi;

// Source for signed distance functions:
// https://iquilezles.org/articles/distfunctions2d/

float opunion(float d1, float d2) {
  return min(d1, d2);
}

float circle(vec2 p, float r) {
  return length(p) - r;
}

float box(vec2 p, vec2 b) {
  vec2 d = abs(p) - b;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float oriented_box(vec2 p, vec2 a, vec2 b, float th) {
  float l = length(b - a);
  vec2 d = (b - a) / l;
  vec2 q = (p - (a + b) * 0.5);

  q = mat2(d.x, -d.y, d.y, d.x) * q;
  q = abs(q) - vec2(l, th) * 0.5;

  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

float segment(vec2 p, vec2 a, vec2 b) {
  vec2 pa = p-a, ba = b-a;
  float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
  return length(pa - ba * h);
}

float parallelogram(vec2 p, float wi, float he, float sk) {
  vec2 e = vec2(sk, he);
  p = (p.y < 0.0) ? -p : p;

  vec2 w = p - e;
  w.x -= clamp(w.x, -wi, wi);

  vec2  d = vec2(dot(w, w), -w.y);
  float s = p.x * e.y - p.y * e.x;
  p = (s < 0.0) ? -p : p;
  vec2  v = p - vec2(wi, 0);
  v -= e * clamp(dot(v, e) / dot(e, e), -1.0, 1.0);

  d = min(d, vec2(dot(v, v), wi * he - abs(s)));
  return sqrt(d.x) * sign(-d.y);
}

vec2 rotate(vec2 v, float radians) {
  float s = sin(radians);
  float c = cos(radians);
  return mat2(c, -s, s, c) * v;
}

float note_stem(vec2 uv) {
  return segment(uv, vec2(0.09, 0.02), vec2(0.09, 0.47)) - 0.005;
}

float note_head(vec2 uv) {
  uv = rotate(uv, 0.4);
  uv.y *= 1.5;
  return circle(uv, 0.1);
}

float note_bar(vec2 uv, float dim) {
  uv.x -= 0.09;
  uv.y -= 0.255;
  return box(uv, vec2(dim, 0.02));
}

float quarter_note(vec2 uv) {
  uv.y += 0.2;
  float d = note_head(uv);
  d = opunion(note_stem(uv), d);
  return d;
}

float eight_note(vec2 uv) {
  float d = quarter_note(vec2(uv.x + 0.2, uv.y));
  d = opunion(d, quarter_note(vec2(uv.x - 0.2, uv.y)));
  d = opunion(d, note_bar(uv, 0.2));
  return d;
}

float triplet_note(vec2 uv) {
  float d = quarter_note(vec2(uv.x + 0.3, uv.y));
  d = opunion(d, quarter_note(vec2(uv.x - 0.0, uv.y)));
  d = opunion(d, quarter_note(vec2(uv.x - 0.3, uv.y)));
  d = opunion(d, note_bar(uv, 0.3));
  return d;
}

float sixteenth_note(vec2 uv) {
  float d = quarter_note(vec2(uv.x + 0.45, uv.y));
  d = opunion(d, quarter_note(vec2(uv.x + 0.15, uv.y)));
  d = opunion(d, quarter_note(vec2(uv.x - 0.15, uv.y)));
  d = opunion(d, quarter_note(vec2(uv.x - 0.45, uv.y)));

  d = opunion(d, note_bar(uv, 0.45));
  d = opunion(d, note_bar(vec2(uv.x, uv.y + 0.09), 0.45));

  return d;
}

void main() {
  vec2 resolution = getResolution();
  vec2 fragCoord = getFragCoord();

  vec2 uv = fragCoord.xy / resolution.xy - 0.5;
  uv.x *= resolution.x / resolution.y;

  float scale = 1.5;
  uv *= scale;

  vec3 background_color = vec3(0.06);
  vec3 foreground_color = vec3(1.0);

  float d = 0.0;
  if (subdiv == 0) d = quarter_note(uv);
  else if (subdiv == 1) d = eight_note(uv);
  else if (subdiv == 2) d = triplet_note(uv);
  else if (subdiv == 3) d = sixteenth_note(uv);

  d = smoothstep(0.03, 0.0, d);
  vec3 c = mix(background_color, foreground_color, d);

#if 0
  if (segment(uv, vec2(-1.0, 0.0), vec2(1.0, 0.0)) < 0.01) c = vec3(1.0, 0.0, 0.0);
  if (segment(uv, vec2(-1.0, 0.25), vec2(1.0, 0.25)) < 0.005) c = vec3(1.0, 0.0, 0.0);
  if (segment(uv, vec2(-1.0, 0.5), vec2(1.0, 0.5)) < 0.005) c = vec3(1.0, 0.0, 0.0);
  if (segment(uv, vec2(-1.0, -0.25), vec2(1.0, -0.25)) < 0.005) c = vec3(1.0, 0.0, 0.0);
  if (segment(uv, vec2(-1.0, -0.5), vec2(1.0, -0.5)) < 0.005) c = vec3(1.0, 0.0, 0.0);
  if (segment(uv, vec2(0.0, -1.0), vec2(0.0, 1.0)) < 0.005) c = vec3(1.0, 0.0, 0.0);
#endif

  o_color = vec4(c, 1.0);
}
)";

}
