// ◦ Xyz ◦
#pragma once

#include<memory>

namespace glider {
	struct Params {
		typedef std::shared_ptr<Params> Ptr;
		float force = 1.25f;
		float height = 40.f;
		float liftingForce = 0.5f;
		float liftingDamping = 0.5f;

	public:
		static const Params::Ptr& GetDefault();

	private:
		static Params::Ptr _defaultPtr;
	};
}
