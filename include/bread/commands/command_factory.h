#pragma once

#include <bread/command.h>

#include <memory>
#include <string>

namespace bread {
namespace commands {

std::unique_ptr<Command> parse_command(const std::string& line);

}  // namespace commands
}  // namespace bread