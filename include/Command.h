#pragma once

class Command
{
public:
    virtual bool execute() = 0;
    virtual bool undo() = 0;

    virtual ~Command() = default;
};