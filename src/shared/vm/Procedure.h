#pragma once

#include "../../common.h"
#include "shared/vm/Object.h"
#include "shared/vm/ProcedureArtifact.h"
#include "shared/vm/ProgramMember.h"

namespace vm {

class Procedure : public ProgramMember {
   public:
    bool isSystemProcedure = false;
    std::optional<std::unique_ptr<ProcedureArtifact>> artifact;

    ProgramMemberType getProgramMemberType() const override;
};

}  // namespace vm
