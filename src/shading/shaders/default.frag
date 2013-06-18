
uniform bool    enable_tex[2];         // Textures enables
uniform sampler2D s_tex0;
uniform sampler2D s_tex1;

// varying variables output by the vertex shader
varying vec2    v_textcoord[2];
varying vec4    v_front_color;
varying vec4    v_back_color;
varying float   v_fog_factor;
varying float   v_ucp_factor;

const float     c_zero= 0.0;
const float     c_one= 1.0;
const int       indx_zero= 0;
const int       indx_one= 1;

void main() {
    if (!enable_tex[indx_zero])
    {
        if (gl_FrontFacing) gl_FragColor= v_front_color;
        else gl_FragColor= v_back_color;
    }
    else
    {
        if (enable_tex[indx_zero])
        {
            if (gl_FrontFacing) gl_FragColor= texture2D(s_tex0, v_textcoord[indx_zero]) * v_front_color;
            else gl_FragColor= gl_FragColor= texture2D(s_tex0, v_textcoord[indx_zero]) * v_back_color;
        }
    }
}
