#include "application.hpp"

#include <iostream>

Application::Application(): Gtk::Application("my.app", Gio::APPLICATION_HANDLES_COMMAND_LINE) {}
Application::~Application() {
    if (this->window != nullptr) {
        delete this->window;
    }
}
bool Application::parse_image_path(const Glib::ustring& option_name, const std::string& value, bool has_value) {
    if (!has_value) {
        std::cerr << "--image option takes in one argument";

        return false;
    }

    if (option_name == "--image" || option_name == "-i") {
        this->image_path = value;

        return true;
    } else {
        std::cerr << "Unable to parse argument: " << option_name << std::endl;

        return false;
    }
}

int Application::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& app_argv) {
    // custom group
    Glib::OptionGroup group("options", "application specific options");
    Glib::OptionEntry entry;
    entry.set_long_name("image");
    entry.set_short_name('i');
    entry.set_flags(Glib::OptionEntry::FLAG_OPTIONAL_ARG);
    entry.set_description("The initial image do be manipulated.");
    entry.set_arg_description("Path to the image file.");
    group.add_entry_filename(entry, sigc::mem_fun3(*this, &Application::parse_image_path));
    
    // add GTK(mm) options, --help-gtk, etc
    Glib::OptionGroup gtk_group(gtk_get_option_group(true));

    // add groups to a context
    Glib::OptionContext context;
    context.add_group(group);
    context.add_group(gtk_group);
    
    // let glib parse
    int argc;
    char** argv = app_argv->get_arguments(argc);
    context.parse(argc, argv);

    if (argc > 1) {
        std::cerr << "Unable to parse argument: " << argv[1] << std::endl;

        return 1;
    }

    this->activate();

    return 0;
}

void Application::on_activate() {
    this->window = new Window();
    this->window->loadImage(this->image_path);
    add_window(*(this->window));
    this->window->show();
}