enum Commands
{
    CmdType,
    CmdLoad,
    CmdMain,
    CmdEmpty,
    CmdUnknown,
    CmdNone,
};

Commands GetCommand(LPCTSTR Cmd);
LPCTSTR GetArgument(LPCTSTR Cmd);
LPCTSTR GetCommandHelp(Commands Cmd);
