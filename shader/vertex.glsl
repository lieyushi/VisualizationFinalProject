const int NUM_LIGHTS = 3;

uniform vec3 cameraPosition;
uniform vec3 lightPosition[NUM_LIGHTS];

varying vec3 fragmentNormal;
varying vec3 cameraVector;
varying vec3 lightVector[NUM_LIGHTS];

void main()
{
    // set the normal for the fragment shader and
    // the vector from the vertex to the camera
    fragmentNormal = gl_Normal;
    cameraVector = cameraPosition - gl_Vertex.xyz;

    // set the vectors from the vertex to each light
    for(int i = 0; i < NUM_LIGHTS; ++i)
        lightVector[i] = lightPosition[i] - gl_Vertex.xyz;

    // output the transformed vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
