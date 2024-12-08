#include <engine/runtime.hpp>

#ifdef EDITOR_BUILD
#include <editor/game_editor.hpp>
#else
#include <game.hpp>
#endif

int RuntimeMain(int argc, char** args)
{
#ifdef EDITOR_BUILD
    SkyPiEditor editor;
    return Application::Get().Run(argc, args, editor);
#else
    SkyPiGame game;
    return Application::Get().Run(argc, args, game);
#endif
}