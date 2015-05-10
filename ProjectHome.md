An object oriented, clean and stable version of the Quake II modding gamex86.dll system.

This project aims to provide a clean, efficient, quick base for any modding you plan to deliver on Quake II. It also provides an easy to use, C++ base.

Windows and Linux are both supported.

Features at a glance:
  * Full use of C++'s features, such as classes, templates and inline functions
  * Original long-standing bugs fixed
  * Object oriented entity system
  * Private server entities that don't take up any network space (deathmatch only)
  * A timer system that can be used to schedule things without using an entity (deathmatch only)
  * Entity-based saving, loading, and fields for level parsing
  * String hashed lookup tables
  * Game API wrapper
  * Several optional addon features
  * "Junk" entity system to reduce the amount of junk on the map at once
  * Full object-oriented standalone item system and weapon system
  * Several optimizations to keep the game running smoother and faster than ever before
  * Aim to offer increased stability over original Quake II
  * Supports any Quake II 3.20 engine
  * A full file system and parsing system
  * A version checker to make sure you're always running the latest version
  * An exception handler that can generate a crash dump for us to examine crashes much more easily
  * The exception handler can also recover from a crash and continue game-play! (Windows)
  * Media system to better handle media indexing
  * Platform-independent classes and functions
  * CTF built-in (dubbed CleanCTF), can be disabled with the CLEANCTF\_ENABLED macro
  * Object oriented math classes, such as vec3f for 3d vectors
  * A custom menu system that has a .NET-similar API
  * Several common controls (Image, Label, Slider, Spin control) and the ability to create custom controls with ease
  * An easier to read error and warning print system for maps
  * A map debug mode (mostly still in development) that can let you edit maps without using a map editor
  * An entity replacement system (.ccent) that supports special preprocessor commands
  * Cvar system to make cvar management easier and faster
  * Banning system to ban people from entering the game, entering spectator mode, or to stop them from talking
  * Object oriented monster system
  * Replacements for nearly all of the game import functions; these replacements are either faster or more secure, or both!
  * Optional deprecation (Visual Studio 2005+ only) for deprecated game import functions, to help you avoid deprecated functions
  * Trace system
  * Easier to manage statusbar system
  * Net optimizations to make sure that data is only sent if it needs to be sent!
  * A "write queue" to allow net optimizations to happen
  * Sound playing optimization to only push one sound per channel for one entity
  * Command argument system to allow easier access to typed subcommands
  * Much, much more! Download the binaries and source for yourself!



&lt;hr/&gt;



&lt;wiki:gadget url="http://www.ohloh.net/p/328735/widgets/project\_languages.xml" border="1" width="360" height="200"/&gt; &lt;wiki:gadget url="http://www.ohloh.net/p/328735/widgets/project\_factoids.xml" border="0" width="500" height="120"/&gt;
&lt;wiki:gadget url="http://www.ohloh.net/p/328735/widgets/project\_basic\_stats.xml" height="220" border="1"/&gt;