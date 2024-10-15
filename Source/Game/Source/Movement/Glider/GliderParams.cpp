// ◦ Xyz ◦
#include"GliderParams.h"

namespace glider {
	Params::Ptr Params::_defaultPtr;

	const Params::Ptr& Params::GetDefault()
	{
		if (!_defaultPtr) {
			_defaultPtr = std::make_shared<Params>();
		}
		return _defaultPtr;
	}
}
