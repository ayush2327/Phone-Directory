#include "CommandManager.h"

bool CommandManager::execute(std::unique_ptr<Command> command)
{
    if (!command || !command->execute())
        return false;

    undoStack.push(std::move(command));

    // A successful new operation invalidates redo history.
    while (!redoStack.empty())
        redoStack.pop();

    return true;
}

bool CommandManager::undo()
{
    if (!canUndo())
        return false;

    auto command = std::move(undoStack.top());
    undoStack.pop();

    if (!command->undo())
    {
        undoStack.push(std::move(command));
        return false;
    }

    redoStack.push(std::move(command));
    return true;
}

bool CommandManager::redo()
{
    if (!canRedo())
        return false;

    auto command = std::move(redoStack.top());
    redoStack.pop();

    if (!command->execute())
    {
        redoStack.push(std::move(command));
        return false;
    }

    undoStack.push(std::move(command));
    return true;
}

bool CommandManager::canUndo() const noexcept
{
    return !undoStack.empty();
}

bool CommandManager::canRedo() const noexcept
{
    return !redoStack.empty();
}