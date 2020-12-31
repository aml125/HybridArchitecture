#pragma once
#include <game/cmp/model.hpp>

namespace GM {
	struct InstantiatedModel_t : Model_t
	{
		/*  Functions   */
		InstantiatedModel_t(std::size_t eid)
			: Model_t{ eid }
		{
		};
	};
}