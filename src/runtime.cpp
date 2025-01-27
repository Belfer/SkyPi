#include <engine/runtime.hpp>
#include <engine/engine.hpp>

#ifdef EDITOR_BUILD
#include <editor/game_editor.hpp>
#else
#include <game.hpp>
#endif

int RuntimeMain(int argc, char** args)
{
#ifdef EDITOR_BUILD
    SkyPiEditor editor;
    return Engine::Get().Run(argc, args, editor);
#else
    SkyPiGame game;
    return Engine::Get().Run(argc, args, game);
#endif
}