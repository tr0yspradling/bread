#include <bread/commands/add_command.h>
#include <bread/commands/append_command.h>
#include <bread/commands/bad_command.h>
#include <bread/commands/command_factory.h>
#include <bread/commands/delete_command.h>
#include <bread/commands/get_command.h>
#include <bread/commands/prepend_command.h>
#include <bread/commands/quit_command.h>
#include <bread/commands/replace_command.h>
#include <bread/commands/set_command.h>
#include <bread/commands/unknown_command.h>

#include <iterator>
#include <sstream>
#include <vector>

std::unique_ptr<bread::commands::Command> bread::commands::parse_command(
    const std::string& line) {
  std::istringstream iss(line);
  std::string cmd;
  iss >> cmd;
  if (cmd == "set") {
    std::string key;
    int flags = 0;
    int exptime = 0;
    size_t bytes = 0;
    iss >> key >> flags >> exptime >> bytes;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<SetCommand>(std::move(key), flags, exptime, bytes);
  } else if (cmd == "get") {
    std::vector<std::string> keys{std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
    if (keys.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<GetCommand>(std::move(keys));
  } else if (cmd == "delete") {
    std::string key;
    iss >> key;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<DeleteCommand>(std::move(key));
  } else if (cmd == "add") {
    std::string key;
    int flags = 0;
    int exptime = 0;
    size_t bytes = 0;
    iss >> key >> flags >> exptime >> bytes;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<AddCommand>(std::move(key), flags, exptime, bytes);
  } else if (cmd == "replace") {
    std::string key;
    int flags = 0;
    int exptime = 0;
    size_t bytes = 0;
    iss >> key >> flags >> exptime >> bytes;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<ReplaceCommand>(std::move(key), flags, exptime,
                                            bytes);
  } else if (cmd == "append") {
    std::string key;
    int flags = 0;
    int exptime = 0;
    size_t bytes = 0;
    iss >> key >> flags >> exptime >> bytes;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<AppendCommand>(std::move(key), flags, exptime,
                                           bytes);
  } else if (cmd == "prepend") {
    std::string key;
    int flags = 0;
    int exptime = 0;
    size_t bytes = 0;
    iss >> key >> flags >> exptime >> bytes;
    if (iss.fail() || key.empty()) {
      return std::make_unique<BadCommand>(
          "CLIENT_ERROR bad command line format");
    }
    return std::make_unique<PrependCommand>(std::move(key), flags, exptime,
                                            bytes);
  } else if (cmd == "quit") {
    return std::make_unique<QuitCommand>();
  }
  return std::make_unique<UnknownCommand>();
}