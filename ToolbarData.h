
TOOLBAR_BEGIN
    TAB_BEGIN("Home", true)
        BUTTON("Run", cmdRun, icoRun)
        BUTTON("Stop", cmdStop, icoStop)
        SEPARATOR
        BUTTON("Open", cmdOpen, icoOpen)
        DROPPER("Recent files", cmdRecent, icoRecent)
    TAB_END
    TAB_BEGIN("Profile", true)
        BUTTON("Run", cmdRunProfile, icoRun)
        BUTTON("Stop", cmdStop, icoStop)
        SEPARATOR
        BUTTON("View", cmdViewProfile, icoClock)
    TAB_END
    TAB_BEGIN("Tools", false)
        BUTTON("Haddock", cmdHaddock, icoWritingDocument)
    TAB_END
TOOLBAR_END
