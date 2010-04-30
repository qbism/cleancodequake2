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
// cc_userinfo.h
// A wrapper class to userinfo strings (provides easy access and caches all strings)
//

#if !defined(CC_GUARD_CC_USERINFO_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_USERINFO_H

struct str_less : std::binary_function <std::string, std::string, bool>
{
	result_type operator() (const first_argument_type &x, const second_argument_type &y) const
	{
		return std::lexicographical_compare(x.c_str(), x.c_str()+x.length()-1, y.c_str(), y.c_str()+y.length()-1);
	}
};

typedef std::map<std::string, std::string, str_less> TUserInfoMap;

class CUserInfo
{
	TUserInfoMap	Info;

	void ParseUserInfo (const std::string &Str);
public:
	CUserInfo () {};
	CUserInfo (const std::string Str)
	{
		ParseUserInfo (Str);
	};
	
	inline void Clear ()
	{
		Info.clear();
	};

	inline bool Valid ()
	{
		return (Info.size() <= 0);
	};

	inline void Update (const std::string UserInfo)
	{
		Clear ();
		ParseUserInfo (UserInfo);
	};

	inline bool HasKey (const std::string Str)
	{
		return (Info.find(Str) != Info.end());
	};

	inline std::string GetValueFromKey (const std::string Str)
	{
		TUserInfoMap::iterator it = Info.find(Str);
		if (it == Info.end())
			return "";

		return (*it).second;
	};

	inline void SetValueForKey (const std::string key, const std::string value)
	{
		Info[key] = value;
	};

	operator std::string ()
	{
		std::string String;

		// Throw the string together
		for (TUserInfoMap::iterator it = Info.begin(); it != Info.end(); ++it)
			String += '\\' + (*it).first + '\\' + (*it).second;

		return String;
	};

	void Save (CFile &File)
	{
		std::string Str = *this;
		File.Write (Str);
	};

	void Load (CFile &File)
	{
		std::string Str = File.ReadCCString ();
		Update (Str);
	};
};

#else
FILE_WARNING
#endif