#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
 
uniform mat4 shadowMatrices[6];
uniform int lightNum; // the nth directional light
 
out vec4 FragPos;
 
void main()
{
    for(int face = 6*lightNum, matIdx=0; matIdx<6; ++face, ++matIdx)
    {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[matIdx] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}