#version 330 core

#define M_PI 3.1415926535897932384626433832795
const int MAX_POINT_LIGHTS = 2;

out vec4 color;

in vec3 fragPosition;
in vec3 fragNormal;

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

uniform float beta; // Thickness of fog
uniform vec3 viewerPos; // Position of the viewer
uniform sampler2D airlightLookup;
uniform vec3 fragColor;
uniform vec3 ColorVec;

const float lightIntensity = 1;
const vec3 lightPos = vec3(5, -5, 10);
const vec3 lightColor = vec3(1, 0, 0);

const vec3 objectDiffuseColor = vec3(0.4, 0.4, 1.0);
const vec3 objectSpecColor    = vec3(0.2, 0.1, 0.1);
const float shininess         = 40;

const vec3 ambientColor = vec3(0.05, 0.05, 0.1);
const vec3 diffuseColor = vec3(0.4, 0.4, 1.0);
const vec3 specColor    = vec3(0.2, 0.1, 0.1);

// Lookup Table Attributes
const float maxU = 10;
const float maxV = M_PI / 2;
const float conversion = 1.5708;

void main() {

    vec3 cumulativeColor = vec3(0, 0, 0);
    vec3 surfaceNormal = normalize(fragNormal);

    vec3 viewerToSurface = fragPosition - viewerPos; // Vector from the viewer to the surface point
    float dvp = length(viewerToSurface); //Distance between surface point and viewer

    // Calculate lighting from point sources
    for(int i = 0; i < numPointLights; i++){
        PointLight p = pointLights[i];
        // Set up initial variables
        // Helper variables
        vec3 viewerToLight = p.position - viewerPos; // Vector from the viewer to the light source
        
        vec3 lightToSurface = fragPosition - p.position; // Vector from the light source to the surface point
        // Distance between light source and viewer
        float dsv = length(viewerToLight);

        float dsp = length(lightToSurface);
        // Optical thickness between light source and viewer
        float tsv = beta * dsv;

        float tsp = beta * dsp;

        // Angle between light source and viewing ray
        float gamma = acos(dot(viewerToLight, viewerToSurface) / (dsv * dvp));
        
        float tvp = beta * dvp; // Optical thickness between light source and surface point

        /****************** Airlight ***********************/

        // The light coming from the direct transmission of the light source.
        vec3 Ld = ((p.base.color) / (dsv * dsv)) * exp(-tsv); // NEED TO ONLY TAKE CONTRIBUTION FROM Direction of source

        // Calculating Airlight Integral
        vec3 A0 = (beta * p.base.color * exp(-tsv * cos(gamma))) / (2 * M_PI * dsv * sin(gamma));
        float A1 = tsv * sin(gamma);
        float v = (M_PI / 4) + (0.5 * atan((tvp - tsv * cos(gamma)) / (tsv * sin(gamma)))); 

        float f1 = texture(airlightLookup, vec2(A1, v)).r;
        float f2 = texture(airlightLookup, vec2(A1, gamma/2)).r;

        vec3 La = A0;// * (f1 - f2); // Airlight Integral

        vec3 totalAirlight = Ld + La;

        /******************* Diffuse ***********************/

        vec3 lightDirection = normalize(-lightToSurface);
        float lambertian = max(dot(lightDirection, surfaceNormal), 0.0);
        vec3 totalLambertian = lambertian * objectDiffuseColor * exp(-tsp) * ((p.base.intensity * 4 * M_PI) / (dsp * dsp));


        /******************* Specular **********************/
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

    // Calculate lighting from directional source
    vec3 dLightDir = normalize(-directionalLight.direction);
    float dLambertian = max(dot(dLightDir, surfaceNormal), 0.0);
    float dSpecular = 0.0;

    if(dLambertian > 0.0) {
        vec3 viewDir = normalize(-fragPosition);

        vec3 halfDir = normalize(dLightDir + viewDir);
        float specAngle = max(dot(halfDir, surfaceNormal), 0.0);

        dSpecular = pow(specAngle, shininess);
    }

    vec3 dFragColor = ambientColor +
        dLambertian * diffuseColor +
        dSpecular * specColor;

    // /cumulativeColor = cumulativeColor + dFragColor;

    color = vec4(cumulativeColor, 1.0f);
}