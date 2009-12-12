/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_modules.cpp
// CleanModule System
//

#if 0
#include "cc_local.h"

struct Module_t
{
	CModule			*Module;
	CDynamicLibrary	*Lib;
};

std::vector <Module_t*, std::generic_allocator<Module_t*> > Modules;

void CC_LoadModule (std::cc_string Name)
{
	CDynamicLibrary *Lib = QNew (com_genericPool, 0) CDynamicLibrary (Name.c_str());
	if (Lib->Valid())
	{
		// Make sure the module is valid
		typedef CModule *(*TLoadModuleAPI) ();
		TLoadLoadModuleAPI;

		Load= Lib->GetFunction<TLoadModuleAPI> (MODULE_ENTRY_POINT);

		if (LoadModuleAPI)
		{
			// We were able to load the module
			// Let's see if it's the right version
			CModule *Module = LoadModuleAPI();

			if (Module && Module->GetAPIVersion() == MODULE_API_VERSION)
			{
				// We're good
				Module_t *Mod = QNew (com_genericPool, 0) Module_t;
				Mod->Lib = Lib;
				Mod->Module = Module;

				Modules.push_back (Mod);
				DebugPrintf ("Loaded module %s\n", Name.c_str());
				return;
			}
			else
			{
				if (!Module)
					DebugPrintf ("WARNING: Module %s is not a valid CleanCode module\n", Name.c_str());
				else
					DebugPrintf ("WARNING: Module %s (%s) does not have the correct API version (%f, should be %f)\n", Name.c_str(), Module->Name.c_str(), Module->GetAPIVersion(), MODULE_API_VERSION);
			}
		}
		else
			DebugPrintf ("WARNING: Module %s is not a valid CleanCode module\n", Name.c_str());
	}
	else
		DebugPrintf ("WARNING: Module %s is not a valid CleanCode module\n", Name.c_str());

	// Failed
	QDelete Lib;
}

void LoadModules ()
{
	DebugPrintf ("\n==== Module Initialization ====\n");
	// Search for DLLs beginning with cc_
	CFindFiles Find (NULL, "cc_*", "dll");

	for (size_t i = 0; i < Find.Files.size(); i++)
		CC_LoadModule (Find.Files[i]);

	if (!Find.Files.size())
		DebugPrintf ("No modules found\n");
	else
		DebugPrintf ("Found %u modules, loaded %u modules\n", Find.Files.size(), Modules.size());

	DebugPrintf ("=================================\n\n");
}

void InitializeModules ()
{
	for (size_t i = 0; i < Modules.size(); i++)
		Modules[i]->Module->Register ();
}

void ShutdownModules ()
{
	for (size_t i = 0; i < Modules.size(); i++)
		Modules[i]->Module->Shutdown ();
}
#endif