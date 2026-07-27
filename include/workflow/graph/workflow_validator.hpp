#pragma once

#include <string>
#include <vector>

#include "workflow/workflow_definition.hpp"

namespace workflow {

struct ValidationResult {
  std::vector<std::string> errors;
  [[nodiscard]] bool valid() const noexcept { return errors.empty(); }
};

class WorkflowValidator {
 public:
  [[nodiscard]] ValidationResult validate(
      const WorkflowDefinition& definition) const;
};

}  // namespace workflow
