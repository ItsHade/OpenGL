#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

out vec3 v_vertexColors;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection;
uniform mat4 u_ViewMatrix;

void main()
{
	v_vertexColors = vertexColors;

	// Transformations are applied for right to left
	vec4 newPosition = u_Projection * u_ViewMatrix * u_ModelMatrix * vec4(position, 1.0f);

	// Don't forget 'w' (can put: gl_Position = newPosition;)
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
};
