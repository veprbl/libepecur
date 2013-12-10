#include <string>

#include <TBranch.h>
#include <TTree.h>

#include "../src/utils/export_tree/TreeExportApp.hpp"

using std::string;

static char	key[MAX_VALUE_LEN];
static char	value[MAX_VALUE_LEN];

static char	*get_info( TTree *info, string lookup_key )
{
	info->GetBranch("key")->SetAddress(key);
	info->GetBranch("value")->SetAddress(value);

	int count = info->GetEntries();
	for(int i = 0; i < count; i++)
	{
		info->GetEntry(i);
		if (key == lookup_key)
		{
			return value;
		}
	}
	return NULL;
}

static TString	get_info_str( TTree *info )
{
	TString	info_str;

	if (!info)
	{
		info_str = "info tree not found. Incomplete file?";
	}
	else
	{
		info_str += get_info(info, "GIT_COMMIT_ID");
		info_str += "; ";
		info_str += get_info(info, "INPUT_FILE");
		info_str += "; ";
		info_str += get_info(info, "GEOMETRY_FILE");
	}

	return info_str;
}
