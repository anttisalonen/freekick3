#include <glob.h>

#include <fstream>

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/gui/LeagueScreen.h"
#include "soccer/gui/LoadGameScreen.h"
#include "soccer/gui/Menu.h"

namespace Soccer {

using namespace Common;

LoadGameScreen::LoadGameScreen(boost::shared_ptr<ScreenManager> sm)
	: Screen(sm)
{
	addButton("Back",          Rectangle(0.35f, 0.85f, 0.30f, 0.10f));

	collectSavedFiles();
	int i = 0;
	for(auto& f : mSavedFiles) {
		addButton(f.c_str(), Rectangle(0.02f + i % 4 * 0.20f,
					0.1f + i / 4 * 0.06f,
					0.18f, 0.05f));
		if(++i / 4 * 0.20f + 0.1f - 0.18f >= 0.85f)
			break;
	}
}

void LoadGameScreen::buttonPressed(boost::shared_ptr<Button> button)
{
	const std::string& buttonText = button->getText();
	if(buttonText == "Back") {
		mScreenManager->dropScreen();
	}
	else {
		std::string filename(Menu::getSaveDir());
		filename += "/";
		filename += buttonText;
		filename += ".sav";
		std::cout << "Opening file " << filename << "\n";
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		in.push(boost::iostreams::bzip2_decompressor());
		in.push(ifs);
		boost::archive::binary_iarchive ia(in);

		boost::shared_ptr<StatefulLeague> league;
		ia >> league;

		mScreenManager->dropScreen();
		mScreenManager->addScreen(boost::shared_ptr<Screen>(new LeagueScreen(mScreenManager, league)));
	}
}

const std::string LoadGameScreen::ScreenName = std::string("Load Game");

const std::string& LoadGameScreen::getName() const
{
	return ScreenName;
}

void LoadGameScreen::collectSavedFiles()
{
	if(!mSavedFiles.empty())
		return;

	glob_t glob_result;

	std::string saveGlob = Menu::getSaveDir();

	saveGlob += "/*.sav";

	glob(saveGlob.c_str(), 0, NULL, &glob_result);
	std::cout << saveGlob << " - " << glob_result.gl_pathc << " results\n";

	for(unsigned int i = 0; i < glob_result.gl_pathc; i++) {
		std::string s1(glob_result.gl_pathv[i]);
		size_t slashpos = s1.find_last_of('/');
		if(slashpos != s1.npos && slashpos < s1.size() + 1) {
			s1.assign(s1, slashpos + 1, s1.size() - slashpos - 5);
			mSavedFiles.push_back(s1);
		}
	}

	globfree(&glob_result);
}

}
