#version 330 core

out vec4 color;

in vec3 fragPosition;
in vec3 fragNormal;
uniform vec3 ColorVec;

const vec3 lightDir = vec3(0.25, 0.25, -1);

const vec3 ambientColor = vec3(0.05, 0.05, 0.1);
const vec3 diffuseColor = vec3(0.4, 0.4, 1.0);
const vec3 specColor    = vec3(0.2, 0.1, 0.1);

const float shininess = 16.0;

void main() {

    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDir);

    float lambertian = max(dot(lightDir,normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0) {
        vec3 viewDir = normalize(-fragPosition);

        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);

        specular = pow(specAngle, shininess);
    }

    vec3 fragColor = ambientColor +
        lambertian * ColorVec +
        specular * specColor;

    color = vec4(fragColor, 1.0);
}