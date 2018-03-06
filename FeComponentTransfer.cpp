#include <vector>
#include <string>
#include <sstream>
#include "windows.h"
#include "avisynth.h"

class AreaResize : public GenericVideoFilter {
	std::vector<float> tableValues;
	float f;

public:
	AreaResize(PClip _child, const char* values, IScriptEnvironment* env) : GenericVideoFilter(_child) {
		std::istringstream is(values);
		std::string s;
		while (is >> s) {
			tableValues.push_back(std::stof(s) * 255);
		}
		f = (float)255 / tableValues.size();
	}
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};

PVideoFrame __stdcall AreaResize::GetFrame(int n, IScriptEnvironment* env) {

	PVideoFrame frame = child->GetFrame(n, env);

	env->MakeWritable(&frame);

	unsigned char* p = frame->GetWritePtr();
	const int pitch = frame->GetPitch();
	const int row_size = frame->GetRowSize();
	const int height = frame->GetHeight();

	if (vi.IsRGB())
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < row_size; x++) {
				//if (x % 3) {	//skip alpha channel
					for (int i = 0; i < tableValues.size(); i++) {
						if (p[x] < f * i) {
							p[x] = tableValues[i];
							break;
						}
					}
				//}
			}

			p += pitch;
		}

	return frame;
}


AVSValue __cdecl CreateAreaResize(AVSValue args, void* user_data, IScriptEnvironment* env) {
    return new AreaResize(args[0].AsClip(), args[1].AsString(), env);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env) {
    env->AddFunction("FeComponentTransfer", "cs", CreateAreaResize, 0);
	std::string result = "AreaResize for AviSynth ";
    return result.c_str();
}
