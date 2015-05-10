CleanCode offers many interchangeable options to suit your project's needs. This list will help you determine which ones you'd want for your specific project.

# cc\_options.h #

## MONSTERS\_ARENT\_STUPID ##
This definition is pretty simplistic; when enabled, monsters will do two more things:
  * Attempt to not shoot allies
  * Won't respond to enemies hitting them by accident

This is all to make them a bit tougher and more realistic. Enabled by default.

## MONSTER\_USE\_ROGUE\_AI ##
This definition will enable the Rogue AI support; smart ducking, sidestepping and a few other features will be disabled when this is not defined. You could remove this if you wanted a simpler AI for something. Enabled by default.

## MONSTERS\_USE\_PATHFINDING ##
CleanCode comes with an Astar pathfinding algorithm to aid monsters in reaching targets. If you don't need this, you can safely remove it. Enabled by default.

## USE\_EXTENDED\_GAME\_IMPORTS ##
Although this may sound useful, "extended game imports" refer to a few game imports that CleanCode doesn't use by default, and has much better replacements for. Disabled by default.

## NO\_DEPRECATING\_OLD\_FUNCTIONS ##
When this is enabled, deprecated functions won't raise a 4996 warning (with Visual Studio 2008).

## CRT\_USE\_UNDEPRECATED\_FUNCTIONS ##
In a few spots in the code, to keep the game clean of buffer overflows, we opted to use Microsoft Visual Studio 7.0+' CRT replacement functions. Enabled by default.

## CC\_USE\_EXCEPTION\_HANDLER ##
Keep this enabled if you want to enable the exception handler. Enabled by default.

## CLEANCTF\_ENABLED ##
Undefining this will remove the CleanCTF gamemode. Note that CleanCTF only takes effect in CTF; in non-CTF, CTF features are non-existant. Enabled by default.

## GAME\_IS\_BEING\_COMPILED\_NOT\_ENGINE\_GO\_AWAY ##
This is a simple macro to remove some things from EGL's shared library that we don't need. Enabled by default.

## MONSTERS\_HIT\_MONSTERSOLID ##
In original Quake2, there are monsterclip brushes which prevent monsters from entering certain areas. While useful for most things, they interfered with pathfinding as several main routes and elevators were cut off by clips. Disabled by default.

## SHARED\_ALLOW\_QUATERNIONS ##
## SHARED\_ALLOW\_3x3\_MATRIX ##
## SHARED\_ALLOW\_4x4\_MATRIX ##
These three are related to the EGL shared library. If you're using a physics library or something that needs matrixes/quats, you may remove these defines. Disabled by default.

## ALLOW\_ASSERTS ##
This define (only valid in Visual Studio) will enable some run-time asserts we have in the game to catch errors that occur within the game. Enabled by default.

## SINCOS\_TABLES ##
This does nothing right now.

## INCLUDE\_GUARDS ##
This definition will enable include guards which prevent the same file from being included more than once. Visual Studio only, since it uses macro'ed pragmas (pragma). Enabled by default.

# cc\_infantry.h #

## INFANTRY\_DOES\_REVERSE\_GUN\_ATTACK ##
This macro enables the Infantry's reverse gun animation. The reverse firing animation makes him much harder as an opponent. Enabled by default.

# cc\_flyer.h #

## FLYER\_KNOWS\_HOW\_TO\_DODGE ##
This enables the flyer's barrel roll dodging animation. Enabled by default.

## SUPERTANK\_USES\_GRENADE\_LAUNCHER ##
This enables the SuperTank's grenade launcher animation. Enabled by default.