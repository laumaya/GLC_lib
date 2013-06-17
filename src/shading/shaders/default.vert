
struct light{
    vec4    position;
    vec4    ambient_color;
    vec4    diffuse_color;
    vec4    specular_color;
    vec3    spot_direction;
    vec3    attenuation_factors;
    float   spot_exponent;
    float   spot_cutoff_angle;
    bool    compute_distance_attenuation;
};

struct material{
    vec4    ambient_color;
    vec4    diffuse_color;
    vec4    specular_color;
    vec4    emissive_color;
    float   specular_exponent;
};

const float     c_zero= 0.0;
const float     c_one= 1.0;
const int       indx_zero= 0;
const int       indx_one= 1;

// Matrix
uniform mat4    modelview_matrix;      // model view matrix
uniform mat4    mvp_matrix;            // Combined model view + projection matrix
uniform mat3    inv_modelview_matrix;  // inverse model view matrix

// to transform normal
uniform mat4    tex_matrix[2];         // Texture matrix
uniform bool    enable_tex[2];         // Textures enables
uniform bool    enable_tex_matrix[2]   // texture matrix enable

// Material and lightnings
uniform         material material_state;
uniform vec4    ambient_scene_color;
uniform light   light_state[8];
uniform bool    light_enable_state[8];
uniform int     num_lights; // number of light enabled

uniform bool    enable_lighting;
uniform bool    light_model_two_sided;
uniform bool    enable_color_material;

uniform bool    enable_fog;
uniform float   fog_density;
uniform float   fog_start, fog_end;
uniform int     fog_mode;

uniform bool    xform_eye_p; // xform_eye_p is set if wee need Peye for user clip plane, lighting or fog

uniform bool    rescale_normal;
uniform bool    normalize_normal;
uniform float   rescale_normal_factor;

uniform vec4    ucp_eqn; // user clip plane equation
uniform bool    enable_ucp;


// vertex attribute - not all of them may be passed in
attribute vec4  a_position;          // this attribute is always specified
attribute vec4  a_textcoord0;        // available if enable_tex[0] is true
attribute vec4  a_textcoord1;        // available if enable_tex[1] is true

attribute vec4  a_color;             // available if !enable_lighting or (enable_lightnig && enable_color_material)
attribute vec3  a_normal;            // available if xform_normal is set (riquired for lighting)

// varying variables output by the vertex shader
varying vec4    v_textcoord[2];
varying vec4    v_front_color;
varying vec4    v_back_color;
varying float   v_fog_factor;
varying float   v_ucp_factor;

// temporary variables used by the vertex shader
vec4            p_eye;
vec3            n;
vec4            mat_ambient_color;
vec4            mat_diffuse_color;

vec4 lighting_equation(int i)
{
    vec4    computed_color= vec4(c_zero, c_zero, c_zero, c_zero);
    vec3    h_vec;
    float   ndot1, ndoth;
    float   att_factor;
    vec3    VPpli;

    att_factor= c_one;
    if (light_state[i].position.w != c_zero)
    {
        // this is a point or a spot light
        float   spot_factor;
        vec3    att_dist;

        // we assume "w" values for PPli and V are the same
        VPpli= light_state[i].position.xyz - p_eye.xyz;
        if (light_state[i].compute_distance_attenuation)
        {
            // Compute distance attenuation
            att_dist.x= c_one;
            att_dist.z= dot(VPpli, VPpli);
            att_dist.y= sqrt(att_dist.z);
            att_factor= c_one / dot(att_dist, light_state[i].attenuation_factors);
        }
        VPpli= normalize(VPpli);

        if (light_state[i].spot_cutoff_angle < 180.0)
        {
            // compute spot factor
            spot_factor= dot(-VPpli, light_state[i].spot_direction);
            if (spot_factor >= cos(radians(light_state[i].spot_cutoff_angle)))
            {
                spot_factor= pow(spot_factor, light_state[i].spot_exponent);
            }
            else
            {
                spot_factor= c_zero;
            }

            att_factor*= spot_factor;
        }
    }
    else
    {
        // this is a directional light
        VPpli= light_state[i].position.xyz;
    }

    if (att_factor > c_zero)
    {
        // Process lighting equation --> compute the light color
        computed_color+= (light_state[i].ambient_color * mat_ambient_color);
        ndot1= max(c_zero, dot(n, VPpli));
        computed_color+= (ndot1 * light_state[i].diffuse_color * mat_diffuse_color);
        h_vec= normalize(VPpli + vec3(c_zero, c_zero, c_one));
        ndoth= dot(n, h_vec);

        if (ndoth > c_zero)
        {
            computed_color+= (pow(ndoth, material_state.specular_exponent) * material_state.specular_color *
                    light_state[i].specular_color);
        }
        computed_color*= att_factor; // multiply color with computed attenuation factor * computed spot factor
    }

    return computed_color;

}

void main() {

}


