#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 13 0
layout(location = 0)
out vec4 entryPointParam_main_outColor_0;


#line 7
layout(location = 0)
in vec3 fragmentIn_fragColor_0;




struct FragmentOut_0
{
    vec4 outColor_0;
};


void main()
{

#line 25
    FragmentOut_0 output_0;
    output_0.outColor_0 = vec4(fragmentIn_fragColor_0, 1.0);

#line 26
    entryPointParam_main_outColor_0 = output_0.outColor_0;

#line 26
    return;
}

