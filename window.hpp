#pragma once

#include <gtkmm.h>

#include "image_proc.hpp"

class Window: public Gtk::Window {
    public:
        Window();
    private:
        static void convertCVtoGTK(const cv::Mat& src, Gtk::Image& dst);

        static void changedMaxAdjustment(bool& blocked, const Glib::RefPtr<Gtk::Adjustment>& min_adj, const Glib::RefPtr<Gtk::Adjustment>& max_adj);
        static void changedMinAdjustment(bool& blocked, const Glib::RefPtr<Gtk::Adjustment>& min_adj, const Glib::RefPtr<Gtk::Adjustment>& max_adj);
        
        void compressionModechange(const image_proc::CompressionMode& mode);
        void orientationChange(const Gtk::StateFlags& flags);
        void changeChannelManipulatorModifier(const image_proc::ModifierOption& option);
        void changeChannelManipulatorChannel(const image_proc::ChannelOption& option);
        void switchEditingMode(Gtk::Widget*, guint page_number);
        void hsvBlockChanged(const Gtk::StateFlags& flags);
        void windowFinishSetup();

        void applyHSVEdits();
        void applyChannelEdits();

        void saveImage();
        void loadImage();

        Glib::RefPtr<Gtk::Adjustment> hue_max_adj, hue_min_adj,
                                      sat_max_adj, sat_min_adj,
                                      val_max_adj, val_min_adj;
        bool hsv_blocked = false,
             hue_blocked = false,
             sat_blocked = false,
             val_blocked = false;

        enum Pages {
            HSV = 0,
            CHANNELS = 1
        };
        image_proc::ModifierOption  current_channel_modifier;
        image_proc::ChannelOption   current_channel_option;
        image_proc::CompressionMode current_compression_mode;
        guint                       current_page_number;

        Gtk::Paned base, left_base;
        Gtk::Switch hv_switch;

        Gtk::Box   images_box;        
        Gtk::Image original_image_widget, altered_image_widget;
        cv::Mat    original_image,        altered_image;
};