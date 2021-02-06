#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform bool gammaCorrection;

void main()
{    
    
    vec3 color = texture(skybox, TexCoords).rgb;
    
    // HDR tonemap and gamma correct
    if(gammaCorrection)
    {
        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0/2.2)); 
    }
    
    FragColor = vec4(color, 1.0);
}