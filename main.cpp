// Approche explicite ou implicite ?

// Template de file d'attente -> pour les oiseaux
//                            -> pour leurs vecteurs

#include <vector>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <functional>
#include <thread> // Pour std::this_thread::sleep_for
#include <chrono> // Pour std::chrono::milliseconds

//#include "vec2.h"
#include "boids.h"


// for initializing and shutdown functions
#include <SDL2/SDL.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

//#include <SDL_image.h>

using namespace std;


// Dimensions de la fenêtre
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Intervalle de mise à jour en millisecondes
const int DT = int(.5f / 60.f * 1000.f); // 100 ms (soit 60 FPS)

const double DT_SEC = 1.f / 60.f;

// Gestion de la caméra

vec2 TOPLEFT_CORNER(0, 0);
vec2 VERTICAL(0, 1);
vec2 HORIZONTAL(1, 0);

void ScrollLeft(double incr) {
	TOPLEFT_CORNER -= incr * HORIZONTAL;
}

void ScrollRight(double incr) {
    TOPLEFT_CORNER += incr * HORIZONTAL;
}

void ScrollUp(double incr) {
	TOPLEFT_CORNER -= incr * VERTICAL;
}

void ScrollDown(double incr) {
    TOPLEFT_CORNER += incr * VERTICAL;
}

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;

float delta_time(Time::time_point t1, Time::time_point t2) {
	fsec fs = t2 - t1;
	return fs.count();
}


// Fonction pour dessiner les points
void drawPoints(SDL_Renderer* renderer, BoidList& boids_l, vec2 topleft_point) {
    // Parcours de chaque point
    boids_l.iter([&](boids* b) {
        vec2 coord = b->get_pos() - topleft_point;
        int x = static_cast<int>(coord.x());
        int y = static_cast<int>(coord.y());

        if (b->is_active) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }

        // Dessiner un point à la position (x, y)
        SDL_RenderDrawPoint(renderer, x, y);
        SDL_RenderDrawPoint(renderer, x + 1, y);
        SDL_RenderDrawPoint(renderer, x - 1, y);
        SDL_RenderDrawPoint(renderer, x, y + 1);
        SDL_RenderDrawPoint(renderer, x, y - 1);
        });
}

void drawBirds(SDL_Renderer* renderer, BoidList& boids_l, vec2 topleft_point, vector<SDL_Texture*> active_list, SDL_Texture* inactive) {
    // Parcours de chaque point
    boids_l.iter([&](boids* b) {
        vec2 coord = b->get_pos() - topleft_point;
        int x = static_cast<int>(coord.x());
        int y = static_cast<int>(coord.y());

        vec2 speed = b->get_speed();
        double angle = (b->get_speed()).argument();
        SDL_Rect dstrect = { x, y, 20, 20};
        if (b->is_active) {
            SDL_RenderCopyEx(renderer, active_list[b->get_behavior()], NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            SDL_RenderCopyEx(renderer, inactive, NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);
        }
        });
}


SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* ren) {
    SDL_Surface* loadedImage = SDL_LoadBMP(file.c_str());
    if (loadedImage != nullptr) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, loadedImage);
        SDL_FreeSurface(loadedImage);
        return texture;
	}
	else {
		std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        return nullptr;
	}
}

vector<int> int_to_color(int hex_code) {
    int r = hex_code / (256 * 256);
    int res = hex_code % (256 * 256);
    int g = res / 256;
    int b = res % 256;
    vector<int>* color = new vector<int>;
    color->push_back(b); color->push_back(g); color->push_back(r);
    return *color;
}

int main(int argc, char* argv[]) {

    // Initialisation de la SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("Animation des points",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    // Création du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    auto ACTIVE_TEXTURE = loadTexture("Sprites/yellow.bmp", renderer);

    auto INACTIVE_TEXTURE = loadTexture("Sprites/grey.bmp", renderer);

    auto CURIOUS_TEXTURE = loadTexture("Sprites/blue.bmp", renderer);

    auto PREDATOR_TEXTURE = loadTexture("Sprites/red.bmp", renderer);

    vector<SDL_Texture*> special_textures = { ACTIVE_TEXTURE, CURIOUS_TEXTURE, PREDATOR_TEXTURE };

    // Liste initiale des positions (vecteur de couples de flottants)
    vector<std::pair<double, double>> pos_l = {
        {200, 200}, {300, 200}, {330, 260}, {300, 320}, {200, 320}, {170, 260}
    };

    BoidList l_boids;

    srand(time(NULL));
    for (auto& point : pos_l) {
        l_boids.push(new boids(point));
    }

    bool running = true;
    SDL_Event event;

    PointList pos = extract_pos(l_boids);
    VecList speed = extract_speed(l_boids);

    
    l_boids.iter([&](boids* b) {
        b->set_wind(vec2(40,0));
        });

    auto t0 = Time::now();
    auto t1 = Time::now();
    auto t2 = Time::now();

    bool show_vectors = false;

    // Boucle principale
    while (running) {
        // Gestion des événements
        vec2 unrelative_position;
        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {

            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    // Mouvement de la caméra
                case SDLK_LEFT:
                    ScrollLeft(10);
                    t0 = Time::now();
                    break;
                case SDLK_RIGHT:
                    ScrollRight(10);
                    t0 = Time::now();
                    break;
                case SDLK_UP:
                    ScrollUp(10);
                    t0 = Time::now();
                    break;
                case SDLK_DOWN:
                    ScrollDown(10);
                    t0 = Time::now();
                    break;
                    // Toggle visuals
                case SDLK_v:
                    if (delta_time(t2, Time::now()) > 0.5) {
                        show_vectors = !show_vectors;
                        t2 = Time::now();
                    }
                    break;
                case SDLK_SPACE:
                    vec2 wind = rnd_sphere_sample()*40;
                    l_boids.iter([&](boids* b) {b->set_wind(wind); });
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                unrelative_position = vec2(event.motion.x, event.motion.y) + TOPLEFT_CORNER;
                boids* candidate = check_clicked(unrelative_position, l_boids);
                if (candidate) {
                    candidate->toggle_activity();
                }
                break;
            }

        }

        t1 = Time::now();

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
        SDL_RenderClear(renderer);

        if (delta_time(t0, t1) > 0.5) {
            l_boids.iter([&](boids* b) {
                b->update(pos, speed, l_boids, DT_SEC);
                });

            // Mise à jour des positions et vitesses
            PointList pos = extract_pos(l_boids);
            VecList speed = extract_speed(l_boids);
            // Pause pour respecter l'intervalle de temps dt
            this_thread::sleep_for(chrono::milliseconds(DT));
        }

        // Dessiner les oiseaux
        drawBirds(renderer, l_boids, TOPLEFT_CORNER, special_textures, INACTIVE_TEXTURE);

        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
