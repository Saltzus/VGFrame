#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 1334 0
struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    vec4  data_0[4];
};


#line 5 1
struct SLANG_ParameterGroup_MatrixBuffer_std140_0
{
    _MatrixStorage_float4x4_ColMajorstd140_0 model_0;
    _MatrixStorage_float4x4_ColMajorstd140_0 view_0;
    _MatrixStorage_float4x4_ColMajorstd140_0 proj_0;
};


#line 1
layout(binding = 0)
layout(std140) uniform _S1
{
    _MatrixStorage_float4x4_ColMajorstd140_0 model_0;
    _MatrixStorage_float4x4_ColMajorstd140_0 view_0;
    _MatrixStorage_float4x4_ColMajorstd140_0 proj_0;
}MatrixBuffer_0;

#line 1
mat4x4 unpackStorage_0(_MatrixStorage_float4x4_ColMajorstd140_0 _S2)
{

#line 1
    return mat4x4(_S2.data_0[0][0], _S2.data_0[1][0], _S2.data_0[2][0], _S2.data_0[3][0], _S2.data_0[0][1], _S2.data_0[1][1], _S2.data_0[2][1], _S2.data_0[3][1], _S2.data_0[0][2], _S2.data_0[1][2], _S2.data_0[2][2], _S2.data_0[3][2], _S2.data_0[0][3], _S2.data_0[1][3], _S2.data_0[2][3], _S2.data_0[3][3]);
}


#line 8
layout(location = 0)
out vec3 entryPointParam_main_fragColor_0;


#line 8
layout(location = 1)
out vec2 entryPointParam_main_fragTexCoord_0;


#line 8
layout(location = 0)
in vec3 vertexIn_inPosition_0;


#line 8
layout(location = 2)
in vec3 vertexIn_inColor_0;


#line 8
layout(location = 3)
in vec2 vertexIn_inTexCoord_0;


#line 8
layout(row_major)
layout(location = 4)
in mat4x4 vertexIn_model_0;


#line 19
struct VertexOut_0
{
    vec4 sv_position_0;
    vec3 fragColor_0;
    vec2 fragTexCoord_0;
};


void main()
{
    VertexOut_0 outVertex_0;
    outVertex_0.fragColor_0 = vertexIn_inColor_0;
    outVertex_0.fragTexCoord_0 = vertexIn_inTexCoord_0;

    outVertex_0.sv_position_0 = (((((((((vec4(vertexIn_inPosition_0, 1.0)) * (vertexIn_model_0)))) * (unpackStorage_0(MatrixBuffer_0.view_0))))) * (unpackStorage_0(MatrixBuffer_0.proj_0))));

    VertexOut_0 _S3 = outVertex_0;

#line 35
    gl_Position = outVertex_0.sv_position_0;

#line 35
    entryPointParam_main_fragColor_0 = _S3.fragColor_0;

#line 35
    entryPointParam_main_fragTexCoord_0 = _S3.fragTexCoord_0;

#line 35
    return;
}

