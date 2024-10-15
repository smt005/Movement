// ◦ Xyz ◦
#pragma once

#include<memory>

namespace glider {
	struct Params {
		typedef std::shared_ptr<Params> Ptr;
		float force = 0.25f;

	public:
		static const Params::Ptr& GetDefault();

	private:
		static Params::Ptr _defaultPtr;
	};
}
