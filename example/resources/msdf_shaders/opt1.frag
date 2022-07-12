#version 330 core

in vec3 texCoord;
in vec4 fgColor;

out vec4 FragColor;

uniform sampler2D msdf;
uniform float fudge;
uniform float gamma;

float median(vec3 v) {
    return max(min(v.r, v.g), min(max(v.r, v.g), v.b));
}

float tap(vec2 offsetUV) {
    float sd = median(texture2D(msdf, offsetUV).rgb);
    float screenPxDistance = fudge * texCoord.z * (sd - 0.5);
    return clamp(screenPxDistance + 0.5, 0.0, 1.0);
}

void main() {
    vec2 dx = dFdx(texCoord.xy);
    vec2 dy = dFdy(texCoord.xy);
    vec2 uvOffsets = vec2(0.125, 0.375);
    vec2 offsetUV = vec2(0.0, 0.0);

    float opacity = 0.0;
    offsetUV.xy = texCoord.xy + uvOffsets.x * dx + uvOffsets.y * dy;
    opacity += tap(offsetUV);
    offsetUV.xy = texCoord.xy - uvOffsets.x * dx - uvOffsets.y * dy;
    opacity += tap(offsetUV);
    offsetUV.xy = texCoord.xy + uvOffsets.y * dx - uvOffsets.x * dy;
    opacity += tap(offsetUV);
    offsetUV.xy = texCoord.xy - uvOffsets.y * dx + uvOffsets.x * dy;
    opacity += tap(offsetUV);
    opacity *= 0.25;

    FragColor = mix(vec4(0.0, 0.0, 0.0, 0.0), fgColor, pow(opacity, 1.0f / gamma));
}
