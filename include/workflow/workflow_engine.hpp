#pragma once

#include <cstddef>
#include <memory>

#include "workflow/observability/logger.hpp"
#include "workflow/storage/state_store.hpp"
#include "workflow/workflow_definition.hpp"
#include "workflow/workflow_result.hpp"

namespace workflow {

class WorkflowEngine {
 public:
  WorkflowEngine(WorkflowDefinition definition,
                 std::unique_ptr<IStateStore> stateStore,
                 std::size_t workerCount, Logger* logger = nullptr);
  [[nodiscard]] WorkflowResult execute();

 private:
  WorkflowDefinition definition_;
  std::unique_ptr<IStateStore> stateStore_;
  std::size_t workerCount_;
  Logger* logger_;
};

}  // namespace workflow
