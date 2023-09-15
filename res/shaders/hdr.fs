#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;

    // Sample color from pre-rendered texture
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    if (hdr) {
        // Reinhard tone mapping (commented out)
        // Maps high dynamic range to low dynamic range, preserving details
        // -------------------------------------
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));

        // Exposure tone mapping
        // ---------------------
        // Darkens bright areas and brightens dark areas based on exposure value
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        
        // Gamma correction
        // ----------------
        // Corrects the brightness to better match human perception
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}