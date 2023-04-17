#pragma once

#include "window.hpp"

class Application: public Gtk::Application {
    public:
        Application();
        ~Application();
    private:
        bool parse_image_path(const Glib::ustring& option_name, const std::string& value, bool has_value);
        int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& app_argv);
        void on_activate();

        Window* window = nullptr;
        std::string image_path = "";
};