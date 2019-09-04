#version 440 core
layout (location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aColor;
layout(location = 1) in vec2 aTexCoord;

//out vec3 outColor;
out vec2 TexCoord;

//Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0f);
   //outColor = aColor;
   TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}