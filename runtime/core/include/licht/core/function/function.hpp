#pragma once

#include <functional>

namespace licht {

template<typename FuncType>
using Function = std::function<FuncType>;

}