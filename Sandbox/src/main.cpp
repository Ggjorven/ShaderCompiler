#include <iostream>
#include <vector>
#include <string_view>

#include <shaderc/shaderc.hpp>

inline constexpr const std::string_view g_Fragment = R"(
// Simple HLSL fragment shader

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    // Gradient: red on the left, blue on the right
    float red  = 1.0 - input.uv.x;
    float blue = input.uv.x;
    return float4(red, 0.0, blue, 1.0);
}

)";

int main(int argc, char* argv[])
{
	(void)argc; (void)argv;

    // Set language
    shaderc::Compiler compiler = {};
    shaderc::CompileOptions options = {};
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
    {
        options.SetSourceLanguage(shaderc_source_language_hlsl);
        options.SetTargetSpirv(shaderc_spirv_version_1_6);

        options.AddMacroDefinition("HLSL");
        options.SetAutoBindUniforms(false);
        options.SetHlslIoMapping(true); // Note: Needed for `register(b0, space0)` layout
    }

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(std::string(g_Fragment), shaderc_fragment_shader, "", options);

    if (!(module.GetCompilationStatus() == shaderc_compilation_status_success))
        std::cerr << "Error compiling shader: " << module.GetErrorMessage() << std::endl;

    // Convert SPIR-V code to vector<char>
    const uint32_t* data = module.cbegin();
    const size_t numWords = module.cend() - module.cbegin();
    const size_t sizeInBytes = numWords * sizeof(uint32_t);
    const char* bytes = reinterpret_cast<const char*>(data);

    std::vector<char> spirv(bytes, bytes + sizeInBytes);
	
    return 0;
}