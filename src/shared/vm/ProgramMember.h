#pragma once

#include "../../common.h"

namespace vm {

enum class ProgramMemberType { kProcedure, kGlobalVariable, kType };

class ProgramMember {
   public:
    std::string name;  // used for display only
    std::optional<std::string> source;

    inline virtual ~ProgramMember() = default;
    virtual ProgramMemberType getProgramMemberType() const = 0;
};

}  // namespace vm
