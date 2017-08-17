
const int NUM_LIGHTS = 3;
const vec3 AMBIENT = vec3(0.1, 0.1, 0.1);
const float MAX_DIST = 2.5;
const float MAX_DIST_SQUARED = MAX_DIST * MAX_DIST;

uniform vec3 lightColor[NUM_LIGHTS];

varying vec3 fragmentNormal;
varying vec3 cemeraVector;
varying vec3 lightVector[NUM_LIGHTS];

void main()
{
    // initialize diffuse/specular lighting
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // normalize the fragment normal and camera direction
    vec3 normal = normalize(fragmentNormal);
    vec3 cameraDir = normalize(cameraVector);

     // loop through each light
    for(int i = 0; i < NUM_LIGHTS; ++i) 
    {
        // calculate distance between 0.0 and 1.0
        float dist = min(dot(lightVector[i], lightVector[i]), MAX_DIST_SQUARED) / MAX_DIST_SQUARED;

         // diffuse
        vec3 lightDir = normalize(lightVector[i]);
        float diffuseDot = dot(normal, lightDir);
        diffuse += lightColor[i] * clamp(diffuseDot, 0.0, 1.0) * distFactor;

        float distFactor = 1.0 - dist;

        // specular
        vec3 halfAngle = normalize(cameraDir + lightDir);
        vec3 specularColor = min(lightColor[i] + 0.5, 1.0);
        float specularDot = dot(normal, halfAngle);
        specular += specularColor * pow(clamp(specularDot, 0.0, 1.0), 16.0) * distFactor;
    }

    vec4 sample = vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(clamp(sample.rgb * (diffuse + AMBIENT) + specular, 0.0, 1.0), sample.a);
}