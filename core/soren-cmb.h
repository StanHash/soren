#ifndef SOREN_CORE_CMB_INCLUDED
#define SOREN_CORE_CMB_INCLUDED

#include <stdexcept>

#include <vector>
#include <string>

#include "core/soren-bytecode.h"

namespace soren {

enum
{
	CMB_SCENE_KIND_FUNCTION = 0,
	CMB_SCENE_KIND_TURN3    = 3,
	CMB_SCENE_KIND_AREA_UNS = 4,
	CMB_SCENE_KIND_TURN6    = 6,
};

struct SceneInfo
{
	unsigned idx { 0u };
	unsigned kind { CMB_SCENE_KIND_FUNCTION };

	std::string name;

	unsigned argCnt { 0u };
	std::vector<int> parameters;

	std::vector<std::string> varnames;

	std::vector<BcIns> rawScript;

	bool isGlobal { false };
};

struct CmbInfo
{
	const char* get_cstr(unsigned offset) const
	{
		if (offset >= stringPool.size())
			throw std::runtime_error("Bad string pool offset");

		return stringPool.data() + offset;
	}

	std::vector<SceneInfo> scenes;
	std::vector<char> stringPool;

	std::vector<std::string> globalNames; // TODO: this may not be what it is, investigate
};

} // namespace soren

#endif // SOREN_CORE_CMB_INCLUDED
