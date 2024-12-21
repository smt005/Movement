// ◦ Xyz ◦
#pragma once

#include<memory>

namespace glider {
	struct Params {
		typedef std::shared_ptr<Params> Ptr;
		float force = 1.925f;
		float height = 30.f;
		float liftingForce = 0.25f;
		float liftingDamping = 0.25f;
		float stabilizationForce = 10.0f;
		float stabilizationDamping = 1.f;

	public:
		static const Params::Ptr& GetDefault();

	private:
		static Params::Ptr _defaultPtr;
	};
}
