#include <TTree.h>

const int MAX_VALUE_LEN = 255;

void make_info_table()
{
	char key[MAX_VALUE_LEN];
	char value[MAX_VALUE_LEN];
	TTree *info = (TTree*)_file0->Get("info");

	info->GetBranch("key")->SetAddress(key);
	info->GetBranch("value")->SetAddress(value);

	cout << "<table border=\"0\">" << endl;
	cout << "<tbody>" << endl;
	int count = info->GetEntries();
	for(int i = 0; i < count; i++)
	{
		info->GetEntry(i);
		cout << "<tr><td>" << key << "</td><td>" << value << "</td></tr>" << endl;
	}
	cout << "</tbody>" << endl;
	cout << "</table>" << endl;
}
