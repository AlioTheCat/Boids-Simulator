// Approche explicite ou implicite ?

// Template de file d'attente -> pour les oiseaux
//                            -> pour leurs vecteurs

#include <vector>
#include <iostream>
#include <cmath>
#include <functional>
#include <thread> // Pour std::this_thread::sleep_for
#include <chrono> // Pour std::chrono::milliseconds

//#include "vec2.h"
#include "boids.h"


// for initializing and shutdown functions
#include <SDL3/SDL.h>

// for using SDL_Delay() functions
#include <SDL3/SDL_timer.h>

#include <SDL3_image/SDL_image.h>

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

/*
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
        SDL_RenderPoint(renderer, x, y);
        SDL_RenderPoint(renderer, x + 1, y);
        SDL_RenderPoint(renderer, x - 1, y);
        SDL_RenderPoint(renderer, x, y + 1);
        SDL_RenderPoint(renderer, x, y - 1);
        });
}*/

void drawBirds(SDL_Renderer* renderer, BoidList& boids_l, vec2 topleft_point, SDL_Texture* active, SDL_Texture* inactive) {
    // Parcours de chaque point
    boids_l.iter([&](boids* b) {
        vec2 coord = b->get_pos() - topleft_point;
        int x = static_cast<int>(coord.x());
        int y = static_cast<int>(coord.y());

        vec2 speed = b->get_speed();
        double angle = (b->get_speed()).argument();
        SDL_FRect dstrect = { x, y, 64, 64};
        if (b->is_active) {
            SDL_RenderTextureRotated(renderer, active, NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);
        }
        else {
            SDL_RenderTextureRotated(renderer, inactive, NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);
        }
        });
}


SDL_Texture* loadTexture(const char* file, SDL_Renderer* ren) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = IMG_Load(file);
    if (surface != nullptr) {
        texture = SDL_CreateTextureFromSurface(ren, surface);
        SDL_DestroySurface(surface);
	}
	else {
		std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
	}
    return texture;
}

int main(int argc, char* argv[]) {
    
	// Initialisation de la SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("Boids Simulator",
        /*SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,*/
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

	// Création du renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    auto ACTIVE_TEXTURE = loadTexture("Sprites/red.bmp", renderer);

    auto INACTIVE_TEXTURE = loadTexture("Sprites/grey.bmp", renderer);

    // Liste initiale des positions (vecteur de couples de flottants)
    std::vector<std::pair<double, double>> pos_l = {
		{200, 200}, {300, 200}, {330, 260}, {300, 320}, {200, 320}, {170, 260}
    };

	BoidList l_boids;

    

    for (auto& point : pos_l) {
        l_boids.push(new boids(point));
    }

    bool running = true;
    SDL_Event event;

    PointList pos = extract_pos(l_boids);
    VecList speed = extract_speed(l_boids);

    l_boids.iter([&](boids* b) {
        b->set_wind(vec2(20, 40));
		b->toggle_activity();
        });

    auto t0 = Time::now();
    auto t1 = Time::now();

    // Boucle principale
    while (running) {
        // Gestion des événements
		bool keydown = true; bool movement = true; vec2 dir_cam;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
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
				default:
					keydown = false;
					break;
                }
            }
        }

        t1 = Time::now();

        if (delta_time(t0, t1)>0.5) {
		    l_boids.iter([&](boids* b) {
			    b->update(pos, speed, DT_SEC);
			    });

		    // Mise à jour des positions et vitesses
            PointList pos = extract_pos(l_boids);
            VecList speed = extract_speed(l_boids);
            // Pause pour respecter l'intervalle de temps dt
            this_thread::sleep_for(chrono::milliseconds(DT));
		}

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Noir
        SDL_RenderClear(renderer);

        // Dessiner les points
        drawBirds(renderer, l_boids, TOPLEFT_CORNER, ACTIVE_TEXTURE, INACTIVE_TEXTURE);

        // Afficher le contenu du renderer
        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
