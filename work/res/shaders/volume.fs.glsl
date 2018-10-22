#version 330 core

#define M_PI 3.1415926535897932384626433832795
const int MAX_POINT_LIGHTS = 6;

out vec4 color;

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragmentColor;
in mat4 model;
in mat4 view;

struct BaseLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    BaseLight base;
    vec3 direction;
};

struct PointLight {
    BaseLight base;
    vec3 position;
};

// Light Related Uniforms
uniform int numPointLights;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Lighting Related Uniforms
uniform float beta;                 // Coefficient representing thickness of fog
uniform vec3 viewerPos;             // Position of the viewer
uniform sampler2D airlightLookup;   // Airlight lookup table
uniform bool onlyPointLights;          // Determine whether only point light contribution should be added.

// Object color attributes
const vec3 objectSpecColor    = vec3(0.2, 0.1, 0.1);
const float shininess         = 20;
const vec3 ambientColor = vec3(0.05, 0.05, 0.1);
const vec3 objectDiffuseColor = vec3(0.4, 0.4, 0.1);
const vec3 specColor    = vec3(0.2, 0.1, 0.1);

// Lookup Table Attributes
const float maxU = 10;
const float maxV = M_PI / 2;
const float conversion = 1.5708;

void main() {

    vec3 cumulativeColor = vec3(0, 0, 0);
    vec3 surfaceNormal = normalize(fragNormal);
    

    /*****************************************
                    Point Lights
    ******************************************/
    for(int i = 0; i < numPointLights; i++){

        PointLight p = pointLights[i]; // Current Point Light

        // Creating a model matrix to transform the lighting
        mat4 lightModelTransform;
        lightModelTransform[0] = vec4(1, 0, 0, 0);
        lightModelTransform[1] = vec4(0, 1, 0, 0);
        lightModelTransform[2] = vec4(0 ,0, 1, 0);
        lightModelTransform[3] = vec4(p.position.x, p.position.y, p.position.z, 1);
        vec4 pos = view * lightModelTransform * vec4(p.position, 1.0f);

        vec3 lightPos = vec3(pos) / pos.w; // Transformed light position
        vec3 viewerToLight = lightPos - viewerPos; // Vector from the viewer to the light source
        vec3 lightToSurface = fragPosition - lightPos; // Vector from the light source to the surface point
        vec3 viewerToSurface = fragPosition - viewerPos;
        float dsv = length(viewerToLight); // Distance between light source and viewer
        float dsp = length(lightToSurface); // Distance between light source and surface point
        float dvp = length(viewerToSurface); //Distance between surface point and viewer
        float tsv = beta * dsv; // Optical thickness between light source and viewer
        float tsp = beta * dsp; // Optical thickness between light source and surface point
        float tvp = beta * dvp; // Optical thickness between light source and surface point
        float gamma = acos(dot(normalize(viewerToLight), normalize(viewerToSurface))); // Angle between light source and viewing ray
        
        /****************** Airlight - Radiance coming from Light Source ***********************/

        // The light coming from the direct transmission of the light source.
        vec3 Ld = ((p.base.color) / (dsv * dsv)) * exp(-tsv);

        // Calculating Airlight Integral
        vec3 A0 = (beta * p.base.color * exp(-tsv * cos(gamma))) / (2 * M_PI * dsv * sin(gamma));
        float A1 = tsv * sin(gamma);
        float v = (M_PI / 4) + (0.5 * atan((tvp - tsv * cos(gamma)) / (tsv * sin(gamma)))); 

        float f1 = texture(airlightLookup, vec2(A1 + 1.0/maxU, v + 1.0/maxV)).r;
        float f2 = texture(airlightLookup, vec2(A1  + 1.0/maxU, gamma/2 + 1.0/maxV)).r;

        vec3 La = A0;// * (f1 - f2);
        vec3 totalAirlight = Ld + La;

        /******************* Diffuse ***********************/

        // Regular diffuse shading attenuated due to optical thickness
        vec3 lightDirection = normalize(-lightToSurface);
        float lambertian = max(dot(lightDirection, surfaceNormal), 0.0);

        vec3 totalLambertian = lambertian * objectDiffuseColor * exp(-tsp) * ((p.base.intensity * 4 * M_PI) / (dsp * dsp));

        /******************* Specular **********************/
        // Regular specular shading attenuated due to optical thickness
        float specular = 0;
        if (lambertian > 0.0) {
            vec3 viewDirection = normalize(-fragPosition);

            vec3 halfDir = normalize(lightDirection + viewDirection);
            float specAngle = max(dot(halfDir, surfaceNormal), 0.0);

            specular = pow(specAngle, shininess);
        }

        vec3 totalSpecular = specular * objectSpecColor * exp(-tsp) * ((p.base.intensity * 4 * M_PI) / (dsp * dsp));

        /*************** Combine Components ****************/
        vec3 totalColor = totalLambertian + totalSpecular + totalAirlight;
        cumulativeColor = cumulativeColor + totalColor;
    }


    /*****************************************
              Directional Light
    ******************************************/
    vec3 dFragColor = vec3(0, 0, 0);
    if(!onlyPointLights) { // Check for bounding box shading. Bounding box only needs to have point light contribtion, not directional.
        
        // Standard directional light calculation.
        vec3 dLightDir = normalize(-directionalLight.direction);
        float dLambertian = max(dot(dLightDir, surfaceNormal), 0.0);
        float dSpecular = 0.0;

        if(dLambertian > 0.0) {
            vec3 viewDir = normalize(-fragPosition);

            vec3 halfDir = normalize(dLightDir + viewDir);
            float specAngle = max(dot(halfDir, surfaceNormal), 0.0);

            dSpecular = pow(specAngle, shininess);
        }

        // Combine directional light components.
        dFragColor = ambientColor +
            dLambertian * fragmentColor +
            dSpecular * specColor;
    }

    // Combine all light contributions
    cumulativeColor = cumulativeColor + dFragColor;

    color = vec4(cumulativeColor, 1.0f);
}