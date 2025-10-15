#include "headers/game.h"

Game::Game(char* file_path) {
    parser = new Parser();
    parser->load_file(file_path);
    current_map = nullptr;
    player = nullptr;
    window = nullptr;
    gl_context = nullptr;
}

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

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return 0;
}

void Game::SelectMap(int id) {
    current_map = parser->generateMap(id);
    if (current_map && current_map->player_start.x != 0 && current_map->player_start.y != 0) {
        // Start above the floor (assuming z is height)
        NewModels::vec3 start_pos = current_map->player_start;
        printf("Player start: %f, %f, %f\nAngle: %d", start_pos.x, start_pos.y, start_pos.z, current_map->player_start_angle);
        start_pos.z += 10; // Adjust based on typical sector height
        player = new Player(start_pos, current_map->player_start_angle); // Match map's angle if available
    } else {
        std::cerr << "No valid player start found for map " << id << std::endl;
        player = new Player({0, 0, 128}, 90.0f); // Fallback
    }
}

void Game::Run() {
    bool running = true;
    SDL_Event event;
    double deltaTime = 0.0;
    double fps = 60.0f;
    double fps_time = 1.0f/fps;
    double render_time = 0.0f;
    uint32_t lastTime = SDL_GetTicks();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
            if (player) {
                player->HandleEvent(&event, deltaTime);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 800, 600);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error after glViewport: " << err << std::endl;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float fov = 60.0f, aspect = 800.0f / 600.0f, nr = 1.0f, fr = 65536.0f;
        float f = 1.0f / tanf(fov * 3.14159f / 360.0f);
        float proj_matrix[16] = {
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (fr + nr) / (nr - fr), -1,
            0, 0, (2 * fr * nr) / (nr - fr), 0
        };
        glLoadMatrixf(proj_matrix);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error after projection setup: " << err << std::endl;
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (render_time>fps_time) {
            if (player) player->Render();
            if (current_map) current_map->render();
            render_time = fmod(render_time, fps_time);
        }

        SDL_GL_SwapWindow(window);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error after SDL_GL_SwapWindow: " << err << std::endl;
        }


        auto now = SDL_GetTicks();
        deltaTime = (now - lastTime) / 1000.0;
        render_time += deltaTime;
        lastTime = now;
    }
}