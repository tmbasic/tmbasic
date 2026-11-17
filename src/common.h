// this is the precompiled header
#ifndef COMMON_H_
#define COMMON_H_

// libabseil_time
#include <absl/time/time.h>
#include <absl/time/civil_time.h>
#include <absl/time/clock.h>

// mpdecimal
#include <decimal.hh>

// tvision
#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TBackground
#define Uses_TButton
#define Uses_TCheckBoxes
#define Uses_TClipboard
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TDrawBuffer
#define Uses_TEditor
#define Uses_TEvent
#define Uses_TFileDialog
#define Uses_TFrame
#define Uses_THardwareInfo
#define Uses_TIndicator
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TLabel
#define Uses_TListBox
#define Uses_TListViewer
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TOutlineViewer
#define Uses_TPalette
#define Uses_TRect
#define Uses_TSItem
#define Uses_TScreen
#define Uses_TScroller
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TStrListMaker
#define Uses_TStringCollection
#define Uses_TStringList
#define Uses_TSubMenu
#define Uses_TText
#define Uses_TWindow
#define Uses_fpstream
#include <tvision/tv.h>
#include <tvision/help.h>

// boost
#include "boost/smart_ptr/local_shared_ptr.hpp"
#include "boost/smart_ptr/make_local_shared.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"

// immer
#define IMMER_NO_THREAD_SAFETY 1  // NOLINT
#include <immer/array.hpp>
#include <immer/array_transient.hpp>
#include <immer/map.hpp>
#include <immer/map_transient.hpp>
#include <immer/set.hpp>
#include <immer/set_transient.hpp>
#include <immer/flex_vector.hpp>
#include <immer/flex_vector_transient.hpp>

// nameof
#define NAMEOF_ENUM_RANGE_MAX 256
#include <nameof.hpp>
#include "shared/SystemCalls.h"

// SystemCall grows as built-ins are added. Keep its reflection range in sync
// without increasing the range scanned for every other enum.
namespace nameof::customize {
template <>
struct enum_range<shared::SystemCall> {
    static constexpr int min = 0;
    static constexpr int max = static_cast<int>(shared::SystemCall::kMaxSystemCall) - 1;
};
}  // namespace nameof::customize

// fmt
#include <fmt/format.h>

// turbo
#include <turbo/turbo.h>
#include <turbo/scintilla/include/SciLexer.h>

// C standard library
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>

// C++ standard library
#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <exception>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <ratio>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#endif

// We ultimately shouldn't need this function if we're using object pooling, but it's easier to have it now as we
// convert from boost::local_shared_ptr to boost::intrusive_ptr.
namespace boost {
template <typename T, typename... Args>
intrusive_ptr<T> make_intrusive_ptr(Args&&... args) {
    // Create the object with new and use perfect forwarding for constructor arguments
    T* ptr = new T(std::forward<Args>(args)...);
    // Wrap the raw pointer in an intrusive_ptr (initial reference count is considered 1)
    return intrusive_ptr<T>(ptr);
}
}  // namespace boost

// clang-tidy likes to see gsl::owner to express ownership of raw pointers, but we don't care about any other part of
// the C++ Guidelines Support Library. let's just define our own gsl::owner.
namespace gsl {
template <class T>
using owner = T;
}  // namespace gsl

#ifdef NDEBUG
#define debugLog(x)
#else
inline void debugLog(std::function<void(std::ofstream* stream)> x) {
    auto fileStream = std::ofstream("debug.log", std::ios::out | std::ios::app);
    x(&fileStream);
    fileStream << "\n";
    fileStream.close();
}
#endif

#endif  // COMMON_H_
