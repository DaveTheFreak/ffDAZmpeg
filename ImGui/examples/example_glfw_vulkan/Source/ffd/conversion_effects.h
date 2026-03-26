#pragma once

#include <string>

#include "parameters.h"
#include "examples/example_glfw_vulkan/Source/Settings/TSettings.h"

namespace ffd::conversion::effects
{
    inline void ChromaticAberration(const TSettings* settings)
    {
        parameters::chromaticAberration = "[0:v]zscale=rin=1:r=1:tin=iec61966-2-1:t=iec61966-2-1,format=gbrp16le";

        if (settings->savedValues.chromaticAberrationIntensity > 0.0f)
        {
            std::string chromatic_aberration_intensity = std::to_string(settings->savedValues.chromaticAberrationIntensity);

            parameters::chromaticAberration += ",extractplanes=r+g+b[r][g][b];";
            parameters::chromaticAberration += "[r]lenscorrection=k1=" + chromatic_aberration_intensity + ":k2=0[r_dist];";
            parameters::chromaticAberration += "[b]lenscorrection=k1=-" + chromatic_aberration_intensity + ":k2=0[b_dist];";
            parameters::chromaticAberration += "[g][b_dist][r_dist]mergeplanes=format=gbrp16le:map0s=0:map0p=0:map1s=1:map1p=0:map2s=2:map2p=0,";

            // Crop out artifacts and scale back to the original resolution (round() prevents 1px deviations)
            parameters::chromaticAberration += "crop=iw*0.995:ih*0.995,zscale=w=round(iw/0.995):h=round(ih/0.995):rin=1:r=1:tin=iec61966-2-1:t=iec61966-2-1,format=rgba64le";
        }
        else
        {
            // Skip CA processing and cropping entirely if intensity is 0.0
            parameters::chromaticAberration += ",zscale=rin=1:r=1:tin=iec61966-2-1:t=iec61966-2-1,format=rgba64le";
        }
    }

    inline void LogoOverlay(const TSettings* settings)
    {
        if (settings->savedValues.logoPath[0] != '\0'
            && std::filesystem::exists(settings->savedValues.logoPath)
            && std::filesystem::is_regular_file(settings->savedValues.logoPath))
        {
            parameters::logoInput = " -i \"" + std::string(settings->savedValues.logoPath) + "\"";

            parameters::chromaticAberration += "[v_ca];";
            std::string logo_logic = "[v_ca]split=2[v_ca_main][v_ca_ref];";
            logo_logic += "[1:v]format=rgba64le[v_logo_raw];";
            logo_logic += "[v_logo_raw][v_ca_ref]scale=w=rw:h=rh[v_logo_scaled];";
            logo_logic += "[v_ca_main][v_logo_scaled]overlay=format=auto";

            parameters::filterChain = parameters::chromaticAberration + logo_logic;
        }
        else
        {
            parameters::filterChain = parameters::chromaticAberration; // Passes output directly without an output pad
        }
    }

    inline void ImageEnhancement(const TSettings* settings)
    {
        if (settings->savedValues.enableImageEnhancement)
        {
            parameters::ditherParam = "-sws_dither ed ";

            int img_width = 1920;
            std::string ffprobe_path = std::string(settings->savedValues.ffmpegExecutablePath) + "\\ffprobe.exe";

            // Execute ffprobe directly without cmd.exe
            std::string probe_cmd =
                "\""
            +   ffprobe_path
            +   "\" -v error -select_streams v:0 -show_entries stream=width -of default=nw=1:nk=1 \""
            +   parameters::inputFile.string()
            +   "\"";

            // CreateProcessA requires a mutable buffer
            std::vector<char> probeCmdBuf(probe_cmd.begin(), probe_cmd.end());
            probeCmdBuf.push_back('\0');

            HANDLE hReadPipe, hWritePipe;
            SECURITY_ATTRIBUTES sa_pipe = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

            if (CreatePipe(&hReadPipe, &hWritePipe, &sa_pipe, 0))
            {
                STARTUPINFOA si_probe;
                PROCESS_INFORMATION pi_probe;
                ZeroMemory(&si_probe, sizeof(si_probe));
                si_probe.cb = sizeof(si_probe);
                si_probe.dwFlags = STARTF_USESTDHANDLES;
                si_probe.hStdOutput = hWritePipe;
                si_probe.hStdError = hWritePipe;
                ZeroMemory(&pi_probe, sizeof(pi_probe));

                if (CreateProcessA(
                    NULL,
                    probeCmdBuf.data(),
                    NULL,
                    NULL,
                    TRUE,
                    CREATE_NO_WINDOW,
                    NULL,
                    NULL,
                    &si_probe, &pi_probe))
                {
                    // Parent MUST close its copy of the write pipe immediately to prevent deadlock
                    CloseHandle(hWritePipe);

                    char buffer[128];
                    DWORD bytesRead;
                    std::string output;

                    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
                    {
                        buffer[bytesRead] = '\0';
                        output += buffer;
                    }

                    WaitForSingleObject(pi_probe.hProcess, INFINITE);
                    CloseHandle(pi_probe.hProcess);
                    CloseHandle(pi_probe.hThread);

                    try { img_width = std::stoi(output); } catch (...) {}
                }
                else
                {
                    CloseHandle(hWritePipe);
                }
                CloseHandle(hReadPipe);
            }

            int deband_range = img_width / 960;
            if (deband_range < 1) deband_range = 1;

            parameters::filterChain += ",deband=1thr=0.01:2thr=0.01:3thr=0.01:range=" + std::to_string(deband_range);
            parameters::filterChain += ",noise=c0s=1:c0f=u";
        }
    }

    namespace hdr
    {
        inline void HdrCurve(const TSettings* settings)
        {
            parameters::hdrCurve = std::format("0/{} 0.1/{} 0.5/{} 0.8/{} 1/{}",
                0.0f + settings->savedValues.hdrCurve[0],
                0.1f + settings->savedValues.hdrCurve[1],
                0.5f + settings->savedValues.hdrCurve[2],
                0.8f + settings->savedValues.hdrCurve[3],
                1.0f + settings->savedValues.hdrCurve[4]);
        }

        inline void HdrDesaturation(const TSettings* settings)
        {
            float primary = 1.0f / 3.0f + (2.0f / 3.0f) * settings->savedValues.hdrSaturation;
            float secondary = 1.0f / 3.0f - (1.0f / 3.0f) * settings->savedValues.hdrSaturation;
            parameters::hdrDesaturation = std::format(
                "colorchannelmixer=rr={0:.2f}:rg={1:.2f}:rb={1:.2f}:"
                "gr={1:.2f}:gg={0:.2f}:gb={1:.2f}:"
                "br={1:.2f}:bg={1:.2f}:bb={0:.2f},",
                primary, secondary);
        }
    }

};
