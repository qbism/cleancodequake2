# First cut at a CMake build script. Get the latest version at www.cmake.org
#
# TODO:
# * Figure out how to give it optimization flags
# * Ensure that it works on both Windows and Linux

# Specify minimum version of CMake required
# This number can be lowered if someone proves it will work in an earlier version
cmake_minimum_required (VERSION 2.8)

# Project name
project (CCQ2)

# Look for header files in these directories
include_directories (shared sircl/include)

# Build the sircl library
add_subdirectory (sircl)

# Find all source files in /shared directory
file (GLOB CC_SHARED_FILES shared *.cpp)

# List all source files in /source directory
# These can't be globbed yet since it will pull in the wrong cc_platform_* files (something to fix during a restructure)
set (CC_SOURCE_FILES source source/cc_arg.cpp source/cc_armor.cpp source/cc_ban.cpp source/cc_barracuda.cpp source/cc_base_entity.cpp source/cc_berserk.cpp source/cc_bfg.cpp source/cc_bitch.cpp source/cc_blaster.cpp source/cc_body_queue.cpp source/cc_boss2.cpp source/cc_brain.cpp source/cc_brush_models.cpp source/cc_chaingun.cpp source/cc_colors.cpp source/cc_commands.cpp source/cc_controls.cpp source/cc_ctf_admin.cpp source/cc_ctf.cpp source/cc_ctf_item_entities.cpp source/cc_ctf_items.cpp source/cc_ctf_menu.cpp source/cc_ctf_weapon_grapple.cpp source/cc_cvar.cpp source/cc_debug_weapons.cpp source/cc_dmflags.cpp source/cc_dummy_entities.cpp source/cc_entity_list.cpp source/cc_entity_types.cpp source/cc_exception_handler.cpp source/cc_file_system.cpp source/cc_floater.cpp source/cc_flyer.cpp source/cc_frontend.cpp source/cc_func_entities.cpp source/cc_game_api.cpp source/cc_game_commands.cpp source/cc_game.cpp source/cc_gladiator.cpp source/cc_grenade_launcher.cpp source/cc_gunner.cpp source/cc_handgrenade.cpp source/cc_hash.cpp source/cc_health.cpp source/cc_hyperblaster.cpp source/cc_icarus.cpp source/cc_indexing.cpp source/cc_infantry.cpp source/cc_info_entities.cpp source/cc_insane.cpp source/cc_inventory.cpp source/cc_irc.cpp source/cc_item_entity.cpp source/cc_itemlist.cpp source/cc_items.cpp source/cc_jorg.cpp source/cc_junk_entities.cpp source/cc_keys.cpp source/cc_machinegun.cpp source/cc_makron.cpp source/cc_makron_stand.cpp source/cc_map_print.cpp source/cc_media.cpp source/cc_medic.cpp source/cc_memory.cpp source/cc_menu.cpp source/cc_misc_entities.cpp source/cc_modules.cpp source/cc_monster_ai.cpp source/cc_monsters.cpp source/cc_mutant.cpp source/cc_parasite.cpp source/cc_player_entity.cpp source/cc_player_trail.cpp source/cc_pmove.cpp source/cc_powerups.cpp source/cc_print.cpp source/cc_railgun.cpp source/cc_rocket_launcher.cpp source/cc_rogue_chainfist.cpp source/cc_rogue_daedalus.cpp source/cc_rogue_disruptor.cpp source/cc_rogue_etf_rifle.cpp source/cc_rogue_flyer_kamikaze.cpp source/cc_rogue_heatbeam.cpp source/cc_rogue_items.cpp source/cc_rogue_medic_commander.cpp source/cc_rogue_misc_entities.cpp source/cc_rogue_monster_ai.cpp source/cc_rogue_monster_spawning.cpp source/cc_rogue_prox_launcher.cpp source/cc_rogue_spheres.cpp source/cc_rogue_stalker.cpp source/cc_rogue_target_entities.cpp source/cc_rogue_tesla.cpp source/cc_rogue_wall_turret.cpp source/cc_rogue_weaponry.cpp source/cc_rogue_widow_stand.cpp source/cc_save.cpp source/cc_server_commands.cpp source/cc_shotgun.cpp source/cc_soldier_base.cpp source/cc_soldier_light.cpp source/cc_soldier_machinegun.cpp source/cc_soldier_shotgun.cpp source/cc_sound.cpp source/cc_status_bar.cpp source/cc_super_shotgun.cpp source/cc_supertank.cpp source/cc_tank.cpp source/cc_target_entities.cpp source/cc_techs.cpp source/cc_temporary_entities.cpp source/cc_timer.cpp source/cc_trace.cpp source/cc_trigger_entities.cpp source/cc_turret_entities.cpp source/cc_userinfo.cpp source/cc_utils.cpp source/cc_version.cpp source/cc_weapon_entities.cpp source/cc_weapon_main.cpp source/cc_weapons.cpp source/cc_write.cpp source/cc_xatrix_chick_heat.cpp source/cc_xatrix_entities.cpp source/cc_xatrix_fixbot.cpp source/cc_xatrix_gekk.cpp source/cc_xatrix_gladiator_phalanx.cpp source/cc_xatrix_ionripper.cpp source/cc_xatrix_items.cpp source/cc_xatrix_phalanx.cpp source/cc_xatrix_soldier_hyper.cpp source/cc_xatrix_soldier_laser.cpp source/cc_xatrix_soldier_ripper.cpp source/cc_xatrix_supertank_boss5.cpp source/cc_xatrix_trap.cpp)

# Set the platform-specific library name and use the correct cc_platform file.
if (WIN32)
  set (CC_LIB_NAME gamex86)
  set (CC_SOURCE_FILES ${CC_SOURCE_FILES} source/cc_platform_win32.cpp)
elseif (UNIX)
  # Get machine hardware name (arch), force 386 (if applicable), strip newlines, and store in variable
  execute_process (COMMAND uname -m COMMAND sed s/i.86/i386/ COMMAND tr -d \n OUTPUT_VARIABLE ARCH)
  set (CC_LIB_NAME game${ARCH})
  set (CC_SOURCE_FILES ${CC_SOURCE_FILES} source/cc_platform_linux.cpp)
else ()
  message (WARNING "Unknown host")
  set (CC_LIB_NAME game)
  set (CC_SOURCE_FILES ${CC_SOURCE_FILES} source/cc_platform_base.cpp)
endif ()

# Specify that the game library consists of source files from /shared and /source directories
add_library (${CC_LIB_NAME} SHARED ${CC_SHARED_FILES} ${CC_SOURCE_FILES})

# Specify that the game library links with sircl and zlib
target_link_libraries (${CC_LIB_NAME} sircl z)

# If host is unix-like, remove "lib" prefix from the library's file name
if (UNIX)
  add_custom_command (TARGET ${CC_LIB_NAME} POST_BUILD COMMAND mv lib${CC_LIB_NAME}.so ${CC_LIB_NAME}.so)
endif ()

