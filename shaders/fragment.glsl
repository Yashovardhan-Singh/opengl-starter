#version 460
out vec4 FragColor;

in vec4 vertexColor;
in vec2 outUV;

uniform sampler2D tex;

void main() {
  FragColor = texture(tex, outUV) * vertexColor;
}
