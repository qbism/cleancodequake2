## Common Project Questions ##

### What is CleanCode? ###
CleanCode is, in summary, a cleaner and stable base for Quake II mods. It provides a fast, C++ interface and remains the cleanest source base to date, even in it's current stage.

### Why move? ###
Moving to CleanCode will offer a plethora of improvements over regular Quake II source bases.
CleanCode promises that your game DLL will perform faster than ever before; with hashing functions and optimized systems, we can guarantee that you won't want to go back!

### But CleanCode is in C++.. doesn't this mean it doesn't work with Quake II engines? ###
Not at all.
C++ and C code can communicate with each other no problem. The game API is left as plain C code for backwards compatibility with any and every Quake II engine.

### So do you have any new particle effects or more max entities? ###
This, among many other things, is still left up to the engine. CleanCode only deals with the game DLL: this includes entities, and things like that that make the game what the game is.

However, if you have an engine that has improved effects and new temporary entities, you can easily change up the macros and make new functions to use these.

### So how do I compile CleanCode? ###
Checkout the SVN source and open the corresponding project file to your IDE. If a project for a certain compiler does not exist, you can help us out by creating it and committing or sending it to us to build into the working copy.

### I'm getting "Find/Load of game library failed" from Quake, what does this mean? ###
This means something failed before the game was able to call GetGameAPI. Normally you're missing something. Base CleanCode does not require any external dependencies, and uses a statically-linked version of the C runtime, therefore the error is usually a result of improper directory structure. If any derivatives require external dependencies they will be listed here.

### I'm trying to convert/write some code similar to original Quake II and I get all of these 4996 warnings.. should I be worried? ###
That depends. If the 4996 warnings are related to CleanCode, follow the advice. This means that I have replaced a specific function and you should, and I repeat, should, use the new, cleaner, more stable function.

I cannot offer support if you intend to use outdated code, however I can help you port older code to CleanCode's new base.

### Why C++? ###
Simple: I wanted the base to be object oriented. There are so many neat features that object oriented programming brings to gaming that would be too much to write in C.

For example, entities have several bases: hurtable entity, thinkable entity, physics entity.. all of these are inherited to make a new entity. Instead of the old method of shoving everything into edict\_s, edict\_s merely stores a pointer to the base class CBaseEntity, which, in object-oriented programming, can be a CBaseEntity or anything that inherits CBaseEntity.

### Why abstract classes? ###
Some classes we didn't intend to be standalone. For example, a CBaseEntity class cannot stand on it's own: it is abstract, and must be inherited and virtual functions filled to make any use of it. You cannot make a new CBaseEntity, however you can make a new class that inherits CBaseEntity.

### I want to port my code over to CleanCode for all these awesome features.. but I've already started working on regular Quake II or I have a mod that I want to port, but it's too hard to do! ###
Sadly, with new features like object orientation, porting becomes harder and harder with each revision. However, if you really want your source ported and are willing to work with a CleanCode team member to port the source code, you can make a deal with us; we'll help you port to CleanCode if you intend to use CleanCode.

### Who made all of this happen? ###
The main programmer, [Paril](mailto:jonno.5000@gmail.com), designed and came up with this idea of a clean, object oriented source base. The idea has been passed around in the past, such as with ioquake2, but this time it is becoming a reality.. very quickly!

Several people also aided in testing and porting projects. Everyone that belongs to this project as an owner or member helped out in some way; whether or not they programmed it, they deserve to be remembered as being a part of CleanCode's reality!

## Gameplay Questions ##

### I downloaded CleanCode, popped in the game, and went to play some single player.. something's extremely wrong here, the monsters are kicking my ass! ###
Note: this question only applies to DLLs built using extra features; the DLL released with CleanCode from here has a macro that disables extra features.

CleanCode offers many improvements that make monsters a bit tougher as a challenge - it's probably the only real gameplay change does. CleanCode implements the Rogue mission pack AI and has a bunch of other goodies to make monsters a challenge more than they ever were before, such as pathfinding and fire prediction!

Rogue AI means they can duck smartly, they will sidestep to avoid shots to the legs, and a few monsters have a few new tricks up their sleeves!

(Notes: Infantry's "backwards firing" animation is NOT from Rogue, this is a completely new change that we made and kept the same model! To disable this firing animation, head to cc\_infantry.h and remove the definition. Same with SuperTank's grenade launcher.)

Pathfinding means that you could make a few monsters angry and leave the area, they could find their way to you! This makes for interesting gameplay since the monsters may not be "shoot-and-forget" anymore!

These features, however, are not permanent; they can be turned off by recompiling the engine with definitions in cc\_options.h

See [Options](Options.md) for more details on each definition and what they do exactly.

### I was doing x and the game crashed! What do I do? ###
Several things can happen during a crash. The two we're looking at here are exception handling; if you see a window come up saying "CleanCode ran into an unhandled exception", move to Fix A of troubleshooting, otherwise go to Fix B. If the game just crashes and gives you the Windows crash message, move to Fix C. If the game and engine disappear with no trace, move to Fix D.

**Fix A**
CleanCode has an exception handler designed to both handle exceptions, generate a crash report, and even in non-severe cases let the game continue normally. When briefed, it is recommended you generate a crash report; after generating the report the game will  attempt to continue normally. If, however, you get a repeat message, we recommend you generate one more crash report to see if they are related; if it happens a third time, end the process. As soon as possible, create an issue on this site, and email us the report(s) (paril at alteredsoftworks dot com).

**Fix B**
If you get a crash report from your engine in particular, post this here as well with the report. You may also send the report to your engine's developer for additional aid. The crash probably didn't get created as a result of CleanCode, but we will double-check to make sure that it wasn't our fault!

**Fix C**
Usually this means that either your engine doesn't have an exception handler, or it crashed somewhere where a crash handler wouldn't of been any help. Submit an issue telling us anything specific you might of done to create the crash. If you have no additional info, still report an issue with your OS, engine choice and any special paks you might have, and we'll try to re-create it.

**Fix D**
This is a bit harder to track, but this might also be an OS issue. Follow all of Step C, and we'll see what we can do.

### Does CleanCode support CTF, Tourney, or anything like that? ###
CleanCode has CTF built in (it's known as CleanCTF internally), so that's not a problem. However, tournament modes, class-based gameplay, all of that would have to be done either yourself or wait for an official release that may contain a tourney-like remake (CleanTourney maybe?). All of these gamemodes are entirely possibly, it just takes someone to code it!

### Is CleanCode R1Q2-protocol compliant? ###
CleanCode does and will follow all of R1CH's great little DLL enhancements that link directly to the protocols he develops. I thank him for his time on R1Q2 and hope he takes a look and encourages the use of CleanCode.

### I tried running CleanCode in KMQuake2 or Quake II Evolved and I get a game API error.. what gives? ###
CleanCode runs on a 3.20 base, therefore the engine must use the 3.20 protocol to work. If you want a certain engine supported, [create an issue](http://code.google.com/p/cleancodequake2/issues/entry) with the "Type-EngineSupport" tag and we'll see what we can do!

## Code Questions ##

### Why do entities have two constructors? Do I always need both? ###
Any entity class you create should have two constructors: Class() and Class(int Index). The difference between the two is that Class() creates a new entity, and Class(int Index) re-uses the entity at spot 'Index'.

You may omit certain constructors under the following conditions:
  * Omit Class() if the entity inherits CMapEntity and should only be made through maps
  * Omit Class(int Index) if the entity does not inherit CMapEntity and will never be created on a re-used spot

### Why am I getting an error on including 

&lt;random&gt;

? ###


&lt;random&gt;

 is from the C++0x Technical Report 1 documentation, which only a handful of compilers support. Among them, which CleanCode is designed around, is Visual Studio 2008 and Visual Studio 2010. Visual Studio 2010 already supports TR1 and some of TR2, so that's not an issue, however VS08 requires the [Visual Studio 2008 Feature Pack](http://www.microsoft.com/downloads/details.aspx?FamilyId=D466226B-8DAB-445F-A7B4-448B326C48E7&displaylang=en), which will give you access to some of the TR1 headers, including random.

If your compiler does not support TR1 at all, or you are not in a position to do anything of the above (such as 2008 Express), you may disable the TR1 headers by setting 