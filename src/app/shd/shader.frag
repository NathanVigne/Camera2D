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
vec4 colormap(float x) {

    float step = 0.125;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;

    if(x<=step){
        r = (47.0/255.0)/step * x;
        g = (50.0/255.0)/step * x;
        b = (140.0/255.0)/step * x;
        return vec4(r,g,b,1.0);
    }

    if(x<=2*step){
        r = -(47.0/255.0)/step * x + (94.0/255.0);
        g = (146.0/255.0)/step * x - (96.0/255.0);
        b = (115.0/255.0)/step * x + (25.0/255.0);
        return vec4(r,g,b,1.0);
    }

    if(x<=3*step){
        r = (90.0/255.0)/step * x - (180.0/255.0);
        g = -(6.0/255.0)/step * x - (208.0/255.0);
        b = -(221.0/255.0)/step * x + (697.0/255.0);
        return vec4(r,g,b,1.0);
    }

    if(x<=4*step){
        r = (165.0/255.0)/step * x - (405.0/255.0);
        g = (35.0/255.0)/step * x + (85.0/255.0);
        b = -(34.0/255.0)/step * x + (136.0/255.0);
        return vec4(r,g,b,1.0);
    }

    if(x<=5*step){
        r = -(15.0/255.0)/step * x + (315.0/255.0);
        g = -(85.0/255.0)/step * x + (565.0/255.0);
        b = (11.0/255.0)/step * x - (44.0/255.0);
        return vec4(r,g,b,1.0);
    }

    if(x<=6*step){
        r = +(15.0/255.0)/step * x + (165.0/255.0);
        g = -(140.0/255.0)/step * x + (840.0/255.0);
        b = -(11.0/255.0)/step * x + (66.0/255.0);
        return vec4(r,g,b,1.0);
    }

    if(x<=7*step){
        r = -(19.0/255.0)/step * x + (369.0/255.0);
        g = (19.0/255.0)/step * x - (114.0/255.0);
        b = (142.0/255.0)/step * x - (852.0/255.0);
        return vec4(r,g,b,1.0);
    }else{
        r = (19.0/255.0)/step * x + (103.0/255.0);
        g = (236.0/255.0)/step * x - (1633.0/255.0);
        b = (113.0/255.0)/step * x - (649.0/255.0);
        return vec4(r,g,b,1.0);
    }

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
                color = colormap(i);
            }
        }
    }
}



