#include "workflow/storage/file_state_store.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <system_error>

namespace workflow {
namespace {

int stateNumber(TaskState state) { return static_cast<int>(state); }

TaskState parseState(int value) {
  if (value < static_cast<int>(TaskState::Pending) ||
      value > static_cast<int>(TaskState::Skipped)) {
    throw std::runtime_error("state file contains an invalid task state");
  }
  return static_cast<TaskState>(value);
}

}  // namespace

FileStateStore::FileStateStore(std::filesystem::path directory)
    : directory_(std::move(directory)) {
  std::filesystem::create_directories(directory_);
}

std::filesystem::path FileStateStore::pathFor(const WorkflowId& id) const {
  if (id.empty() || id.find_first_of("/\\") != std::string::npos ||
      id == "." || id == "..") {
    throw std::invalid_argument("workflow id is not safe for file storage");
  }
  return directory_ / (id + ".state");
}

void FileStateStore::save(const WorkflowSnapshot& snapshot) {
  std::lock_guard lock(mutex_);
  const auto destination = pathFor(snapshot.workflowId);
  auto temporary = destination;
  temporary += ".tmp";
  {
    std::ofstream output(temporary, std::ios::trunc);
    if (!output) {
      throw std::runtime_error("cannot open state file for writing");
    }
    output << "WORKFLOW_STATE_V1 " << std::quoted(snapshot.workflowId) << '\n';
    for (const auto& [id, task] : snapshot.tasks) {
      output << std::quoted(id) << ' ' << stateNumber(task.state) << ' '
             << task.attempts << ' ' << std::quoted(task.error) << '\n';
    }
    output.flush();
    if (!output) {
      throw std::runtime_error("failed while writing state file");
    }
  }
  std::error_code error;
  std::filesystem::rename(temporary, destination, error);
  if (error) {
    std::filesystem::remove(destination, error);
    error.clear();
    std::filesystem::rename(temporary, destination, error);
  }
  if (error) {
    throw std::runtime_error("cannot replace state file: " + error.message());
  }
}

std::optional<WorkflowSnapshot> FileStateStore::load(
    const WorkflowId& workflowId) {
  std::lock_guard lock(mutex_);
  std::ifstream input(pathFor(workflowId));
  if (!input) {
    return std::nullopt;
  }
  std::string signature;
  WorkflowSnapshot snapshot;
  if (!(input >> signature >> std::quoted(snapshot.workflowId)) ||
      signature != "WORKFLOW_STATE_V1" ||
      snapshot.workflowId != workflowId) {
    throw std::runtime_error("invalid state file header");
  }
  TaskId id;
  int state = 0;
  TaskSnapshot task;
  while (input >> std::quoted(id) >> state >> task.attempts >>
         std::quoted(task.error)) {
    task.state = parseState(state);
    if (!snapshot.tasks.emplace(id, task).second) {
      throw std::runtime_error("duplicate task in state file");
    }
  }
  if (!input.eof()) {
    throw std::runtime_error("corrupt state file");
  }
  return snapshot;
}

}  // namespace workflow
