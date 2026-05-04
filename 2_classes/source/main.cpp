#include "app_mode.hpp"

#include <vector>
#include <cstring>
#include <print>
#include <string_view>
#include <optional>

// -cli for cli mode
int main(int argv, char** argc) {
	// Allowed argument holders
	enum class mode_t {
		null,
		cli,
		gui
	};
	std::optional<mode_t> mode;
	std::optional<bool> want_help;

	// Vectorized CMD line
	std::vector<std::string_view> args(argv);
	// Parsing
	for(auto& it : args) {
		it = std::string_view(*argc, std::strlen(*argc));
		++argc;
	}

	// CMD line argument capturing
	for(auto& it : args) {
		using namespace std::string_view_literals;

		if(it.rfind("-M"sv, 0) != std::string_view::npos) {
			if(it == "-Mcli"sv)
				mode = mode_t::cli;
			else if(it == "-Mgui"sv)
				mode = mode_t::gui;
		#ifdef DEBUG
		if(mode)
			std::print("IO mode set: {}\n", mode==mode_t::gui ? "gui"sv : "cli"sv);
		else
			std::print("IO mode is not set!\n");
		#endif
		}
		else if(it == "-h"sv)
			want_help = true;
	}

	// Defaults
	if(!mode)
		mode = mode_t::cli;
	if(!want_help)
		want_help = false;

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
