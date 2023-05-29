#version 330 core
uniform usampler2D myTexture;
flat in int cChoice;
flat in int mVal;
in vec2 texc;
flat in uvec2 line;
out vec4 color;
float i;
// function OK for color map heat !!
// Need to adjust the transition to white (threshold and speed)
// take the color transition to be 2-5 increment of the normalized bit depth ?
float colormap_red(float x) {
        return 4.0 * x - 2;
}

float colormap_green(float x, float normStep) {
    if (x < 0.5) {
        return 4.0 * x - 1.0;
    } else {
        if(x<(1-3*normStep)){
            return -4.0 * x + 4.0;
        }else{
            return 1/(3*normStep) * x - 1;
        }
    }
}

float colormap_blue(float x, float normStep) {
    if (x < 0.375) {
       return 4.0 * x + 0.0;
    } else {
        if(x<(1-3*normStep)){
            return -4.0 * x + 3.0;
        }else{
            return 1/(3*normStep) * x - 1;
        }
    }
}

vec4 colormap(float x, float normStep) {
    float r = clamp(colormap_red(x), 0.0, 1.0);
    float g = clamp(colormap_green(x, normStep), 0.0, 1.0);
    float b = clamp(colormap_blue(x, normStep), 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}

void main(void)
{

    float norm_step = 1.0/float(mVal);

    // X / Y lines
    if(abs(gl_FragCoord.x - line.x) < 1){
        color = vec4(1.0,0.0,0.0,1.0);
    }else{
        if(abs(gl_FragCoord.y - line.y) < 1  ){
        color = vec4(0.0,0.0,1.0,1.0);
        }else{
            i = texture(myTexture, texc).r / float(mVal);
            if(cChoice==0){
                // Monochrome
                color = vec4(i,i,i,1.0);
            }else if(cChoice==1){
                // Monochrome + red sat (valeur de coupure a ajuster !)
                if(i>=(1-1*norm_step)){
                    color = vec4(1.0,0.0,0.0,1.0);
                }else{
                    color = vec4(i,i,i,1.0);
                }
            }else{
                // Heat map + white sat (valeur de coupure a ajuster !)
                color = colormap(i, norm_step);
            }
        }
    }
}



