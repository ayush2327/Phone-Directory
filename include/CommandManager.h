#pragma once

#include "Command.h"

#include <memory>
#include <stack>

class CommandManager
{
private:
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;

public:
    bool execute(std::unique_ptr<Command> command);

    bool undo();
    bool redo();

    bool canUndo() const noexcept;
    bool canRedo() const noexcept;
};