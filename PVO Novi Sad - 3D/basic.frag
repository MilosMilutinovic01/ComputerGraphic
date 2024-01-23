#version 330 core
out vec4 FragColor;

struct Light {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

#define NR_POINT_LIGHTS 6

in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;
   
uniform vec3 viewPos; 
uniform Material material;
uniform Light light;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform float blink;
uniform float pointLightVisibility;
uniform float transparency;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 ambient = light.ambient * texture(material.diffuse, chUV).rgb;
  	
    vec3 norm = normalize(chNormal);
    vec3 viewDir = normalize(viewPos - chFragPos);

    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, chUV).rgb;  
    
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, chUV).rgb;  
        
    vec3 result = ambient + diffuse + specular;
    result += CalcPointLight(pointLights[0], norm, chFragPos, viewDir);
    for(int i = 1; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, chFragPos, viewDir) * blink;
    //FragColor = vec4(result.r + blink, result.g + blink, result.b + blink, 1.0);
    result += CalcSpotLight(spotLight, norm, chFragPos, viewDir);
    FragColor = vec4(result, transparency);
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(pointLight.position - fragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, chUV));
    vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, chUV));
    vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, chUV));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    vec3 result = ambient + diffuse + specular;
    return result * pointLightVisibility;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(spotLight.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(spotLight.position - fragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    
    float theta = dot(lightDir, normalize(-spotLight.direction)); 
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = spotLight.ambient * vec3(texture(material.diffuse, chUV));
    vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material.diffuse, chUV));
    vec3 specular = spotLight.specular * spec * vec3(texture(material.specular, chUV));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}