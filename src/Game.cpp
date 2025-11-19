#include "headers/game.h"
#include "headers/HudRender.h"

uint16_t Game::screen_width = 800;
uint16_t Game::screen_height = 600;

Game::Game(char* file_path) {
    parser = new Parser();
    parser->load_file(file_path);
    current_map = nullptr;
    player = nullptr;
    window = nullptr;
    gl_context = nullptr;
    std::vector<DoomGunInterface*> Weapons;
}

Game::Game(char* file_path, char** patch_files){}

Game::~Game() {
    delete player;
    delete parser;
    if (gl_context) SDL_GL_DeleteContext(gl_context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int Game::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(
        "Doom Map",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    if (SDL_GL_MakeCurrent(window, gl_context) < 0) {
        std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    HudRender::LoadHudTextures();

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return 0;
}

void Game::SelectMap(int id) {
    current_map = parser->generateMap(id);
    if (current_map && current_map->player_start.x != 0 && current_map->player_start.y != 0) {
        // Start above the floor (assuming z is height)
        svec3 start_pos = current_map->player_start;
        printf("Player start: %hd, %hd, %hd\nAngle: %d", start_pos.x, start_pos.y, start_pos.z, current_map->player_start_angle);
        start_pos.z += 10; // Adjust based on typical sector height
        player = new Player(start_pos, current_map->player_start_angle, current_map); // Match map's angle if available
    } else {
        std::cerr << "No valid player start found for map " << id << std::endl;
        player = new Player({0, 0, 128}, 90.0f, current_map); // Fallback
    }
}

DoomGunInterface* Game::LoadWeapon(DoomGunInitiator initiator) {
    const double tick = 1/35.;
    TexBinder* tb = current_map->texture_binder;
    DoomGunInterface* weapon = new DoomGunInterface();
    weapon->SetDelay(initiator.pickupDelay);

    std::vector<animationFrame> animations[3] = {};
    for (int i = 0; i < 3; i++) {
        std::vector<animationFrame> frames;
        for (auto &frame: initiator.loaders[i]) {
            gl_texture anim_frame = tb->GetTexture((initiator.base_texture_name + frame.let+"0").c_str(), TextureType::WeaponTexture);
            frames.push_back(animationFrame{anim_frame,frame.time*tick});
        }
        animations[i]=frames;
    }
    weapon->SetAnimationFrames(animations);

    std::vector<animationFrame> flashFrames;
    for (auto &frame: initiator.flashLoader) {
        gl_texture anim_frame = tb->GetTexture((initiator.flash_texture_name + frame.let + "0").c_str(), TextureType::WeaponTexture);
        flashFrames.push_back(animationFrame{anim_frame,frame.time*tick});
    }

    weapon->SetFlashFrames(flashFrames);
    return weapon;
}

void Game::GenerateWeapons() {
    Weapons = {
        nullptr,
        nullptr,
        LoadWeapon(Pistol),
        LoadWeapon(Shotgun),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };
}

void Game::Run() {
    GenerateWeapons();
    player->BindWeapons(Weapons);
    bool running = true;
    SDL_Event event;
    double deltaTime = 0.0;
    double fps = 120.0f;
    double fps_time = 1.0f / fps;
    double render_time = 0.0f;
    uint32_t lastTime = SDL_GetTicks();
    ///TODO -- xxx
    SDL_SetRelativeMouseMode(SDL_TRUE);
    bool size_changed = true;  // Force initial projection setup

    while (running) {
        // printf("start\n");
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
            if (player && (event.type == SDL_MOUSEMOTION  || event.type == SDL_MOUSEWHEEL)) {
                player->HandleEvent(&event, deltaTime);
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                screen_width = event.window.data1;
                screen_height = event.window.data2;
                size_changed = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                player->TryShoot();
        }

        if (player) {
            player->HandleEvent();
            player->Update(deltaTime);
        }

        if (current_map) current_map->Update(deltaTime);

        // Set viewport consistently at the start of each frame
        glViewport(0, 0, screen_width, screen_height);

        // Update projection if size changed
        if (size_changed) {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            float fov = 60.0f;
            float aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);
            float nr = 1.0f;
            float fr = 65536.0f;
            float f = 1.0f / tanf(fov * 3.14159f / 360.0f);
            float proj_matrix[16] = {
                f / aspect, 0, 0, 0,
                0, f, 0, 0,
                0, 0, (fr + nr) / (nr - fr), -1,
                0, 0, (2 * fr * nr) / (nr - fr), 0
            };
            glLoadMatrixf(proj_matrix);

            glMatrixMode(GL_MODELVIEW);
            size_changed = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (render_time > fps_time) {

            if (player) player->Render();
            if (current_map) current_map->Render();

            HudRender::Render();
            render_time = fmod(render_time, fps_time);
        }



        SDL_GL_SwapWindow(window);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error after SDL_GL_SwapWindow: " << err << std::endl;
        }

        auto now = SDL_GetTicks();
        deltaTime = (now - lastTime) / 1000.0;
        render_time += deltaTime;
        lastTime = now;

        // printf("Dt:%f\n", deltaTime);
    }
}

void Game::GetScreenSize(uint16_t &width, uint16_t &height){
    width = screen_width;
    height = screen_height;
}
