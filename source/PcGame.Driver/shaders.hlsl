cbuffer CameraBuffer : register(b0)
{
    row_major matrix view;
    row_major matrix projection;
};

cbuffer ModelBuffer : register(b1)
{
    row_major matrix scale;
    row_major matrix rotation;
    row_major matrix translation;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput result;
    
    matrix world = mul(mul(scale, rotation), translation);
    matrix wvp = mul(mul(world, view), projection);
    
    result.position = mul(float4(input.position, 1.0f), wvp);
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}