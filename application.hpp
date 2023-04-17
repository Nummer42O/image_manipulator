#pragma once

#include "window.hpp"

class Application: public Gtk::Application {
    public:
        /**
         * Instanciate Gtk::Application to "image_manipulator.main" with the APPLICATION_HANDLES_COMMAND_LINE flag.
        */
        Application();

        /**
         * Delete the associated window pointer.
        */
        ~Application();
    private:
        /**
         * Check wether the argument given behind --image/-i is valid or even existing.
         * 
         * (internal)
         * 
         * @param option_name: name of the option (--image/-i)
         * @param value: value of the parameter, if any
         * @param has_value: wether or not value is set
         * @return wether or not the argument is valid / the option can be parsed
        */
        bool parse_image_path(const Glib::ustring& option_name, const std::string& value, bool has_value);

        /**
         * Callback for general command line argument processing.
         * 
         * (internal)
         * 
         * @param app_argv: Gio::ApplicationCommandLine reference pointer to access given command line options
         * @return exit code
        */
        int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& app_argv);

        /**
         * Callback to be activated when Application::activate gets called.
         * This adds the main window and initializes it.
         * 
         * (internal)
        */
        void on_activate();


        Window* window = nullptr;
        // storage for image option argument
        std::string image_path = "";
};