#include "cli_mode.hpp"

#include <vector>
#include <cstring>
#include <print>

// -cli for cli mode
int main(int argv, char** argc) {
	enum class mode_t {
		cli,
		gui
	} mode;

	std::vector<std::string_view> args(argv);

	for(auto& it : args) {
		it = std::string_view(*argc, std::strlen(*argc));
		++argc;
	}

	for(auto& it : args) {
		if(it.find("-M")) {
			if(it == "-Mcli")
				mode = mode_t::cli;
			else if(it == "-Mgui")
				mode = mode_t::gui;
		#ifndef DEBUG
		std::print("IO mode set: {}", static_cast<long>(mode));
		#endif
		}
	}


	if(mode == mode_t::gui) {
		//gui::gui_mode app;

		//while(app.refresh_and_wait());
	}
	else if(mode == mode_t::cli) {
		cli::cli_mode app;

		while(app.refresh_and_wait());
	}

	return 0;
}
