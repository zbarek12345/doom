#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "map.h"
#include "new_models.h"
#include "Weapon.h"

class Player {
public:
   static uint8_t current_weapon;

   static bool next_weapon_selected;

   static double next_weapon_selected_timer;

   Player(svec3 position, float angle, NewModels::Map* map);
   void Update(double deltaTime);
   void HandleEvent(SDL_Event* event, double deltaTime);
   void HandleEvent();
   void Render();

   static bool CanChangeWeapon();

   static void SelectPreviousWeapon();

   static void SelectNextWeapon();

   static bool GetCurrentFlashFrame(gl_texture& frame);
   static bool GetCurrentWeaponFrame(gl_texture& frame);

   static WeaponType GetCurrentWeaponType();

   static bool TryPickWeapon(uint8_t weapon_index);

   static uint8_t GetCurrentAmmoType();

   static void TryShoot();

   static bool has_backpack;
   static int16_t health; static int16_t armor;
   ///[0] - Bull, [1] - Shell, [2] - Miss, [3] - Cell
   static uint16_t ammo[4];
   static uint16_t max_ammo[4];
   static bool has_weapon[9];
   static void BindWeapons(std::vector<DoomGunInterface*> Weapons);
private:
   fvec3 movement_vector{};
   NewModels::Map* new_map;
   svec3 position{}; // Short for map compatibility
   fvec3 pos;// Float for smooth movement
   Camera* camera;
   original_map * map;
   NewModels::Sector * current_sector;
   bool ray_launched;
   static std::vector<DoomGunInterface*> weapons;
};

#endif