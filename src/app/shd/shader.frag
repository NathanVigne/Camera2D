#version 330 core
//uniform usampler2D myTexture;
//in mediump vec2 texc;
//void main(void)
//{
//  float textureColor = texture(myTexture, texc).r/65535.0;
//   gl_FragColor = vec4(textureColor,textureColor,textureColor,1.0);
//}


uniform usampler2D myTexture;
in vec2 texc;
flat in uvec2 line;
//out vec4 colorX;
//out vec4 colorY;
out vec4 color;

void main(void)
{

    if(abs(gl_FragCoord.x - line.x) < 1){
        color = vec4(1.0,0.0,0.0,1.0);
    }else{
        if(abs(gl_FragCoord.y - line.y) < 1  ){
        color = vec4(0.0,0.0,1.0,1.0);
        }else{
            float r = texture(myTexture, texc).r / 1022.0;
            color = vec4(r,r,r,1.0);
        }
    }

//    float x = line.x/500.0;
//    float y = line.y/500.0;
//    color = vec4(x,y,0.0,1.0);
}
