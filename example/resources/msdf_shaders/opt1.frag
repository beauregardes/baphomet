#version 330 core

in vec3 texCoord;
in vec4 fgColor;

out vec4 FragColor;

uniform sampler2D msdf;
uniform float fudge;
uniform float med_adj1;
uniform float med_adj2;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 s = texture(msdf, texCoord.xy).rgb;
    float sigDist = fudge * texCoord.z * (median(s.r, s.g, s.b) - med_adj1);
    float opacity = clamp(sigDist + med_adj2, 0.0, 1.0);
    FragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), fgColor, opacity);
}
