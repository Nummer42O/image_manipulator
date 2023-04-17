#pragma once

#include <gtkmm.h>

#include "image_proc.hpp"

class Window: public Gtk::Window {
    public:
        /**
         * Initialize the main window by setting up the widgets.
        */
        Window();
        
        
        /**
         * Load an image externally.
         * Main use is to load an initial image.
         * 
         * @param filepath: path to the image
        */
        void loadImage(const std::string& filepath);
    private:
        /**
         * Convert an image from a cv::Mat to an Gtk::Image.
         * 
         * @param src: source image to be converted (RGB, 3channel, 8bit)
         * @param dst: resulting Gtk image (existing pixbuf will be overwritten)
        */
        static void convertCVtoGTK(const cv::Mat& src, Gtk::Image& dst);


        /**
         * Callback for when a maximum scale gets adjusted.
         * This will lower then minimum value, if it is above the new maximum.
         * 
         * @param blocked: blocker variable to inhibit changedMinAdjustemtn to be called, when the min value gets adjusted
         * @param min_adj: reference to the minimum adjustment to get and set values
         * @param max_adj: reference to the maximum adjustment to get and set values 
        */
        static void changedMaxAdjustment(bool& blocked, const Glib::RefPtr<Gtk::Adjustment>& min_adj, const Glib::RefPtr<Gtk::Adjustment>& max_adj);

        /**
         * Callback for when a minimum scale gets adjusted.
         * This will raise then maximum value, if it is below the new minimum.
         * 
         * @param blocked: blocker variable to inhibit changedMinAdjustemtn to be called, when the min value gets adjusted
         * @param min_adj: reference to the minimum adjustment to get and set values
         * @param max_adj: reference to the maximum adjustment to get and set values 
        */
        static void changedMinAdjustment(bool& blocked, const Glib::RefPtr<Gtk::Adjustment>& min_adj, const Glib::RefPtr<Gtk::Adjustment>& max_adj);
        

        /**
         * Callback for a change in the compression mode group.
         * 
         * @param mode: new mode to be set
        */
        void compressionModechange(const image_proc::CompressionMode& mode);

        /**
         * Callback for a change in the image orientation.
         * This changed if the images are besides or on top of each other.
         * 
         * @param <unused>
        */
        void orientationChange(const Gtk::StateFlags&);

        /**
         * Callback for a change in channel modifier option group.
         * Channel modifier options mean a channel or combination of channels that are used for the resulting image.
         * 
         * @param option: new channel modifier option to be set
        */
        void changeChannelManipulatorModifier(const image_proc::ModifierOption& option);

        /**
         * Callback for a change in channel option.
         * Channel options mean which channels are to be used as output channels for the resulting image.
         * 
         * @param option: new channel option to be set
        */
        void changeChannelManipulatorChannel(const image_proc::ChannelOption& option);

        /**
         * Callback for a change in editing notebook tab.
         * 
         * @param <unsused>
         * @param page_number: number of the current tab/page
        */
        void switchEditingMode(Gtk::Widget*, guint page_number);

        /**
         * Callback for the HSV blocking toggle.
         * This toggle allows for changed in the HSV adjustments without triggering a constant reapplication to the image.
         * 
         * @param flags: (new) state flags of the switch
        */
        void hsvBlockChanged(const Gtk::StateFlags& flags);

        /**
         * Callback to be called once when the window is first idling, ergo when Gtk completely finished its setup.
         * This method will set the paned windows to reasonable positions.
        */
        void windowFinishSetup();


        /**
         * Callback to apply the changes made in HSV tab.
         * Also applies compression.
        */
        void applyHSVEdits();

        /**
         * Callback to apply the changed made in Channels tab.
         * Also applies compression.
        */
        void applyChannelEdits();


        /**
         * Callback to save the image into a chosen location.
        */
        void saveImage();

        /**
         * Callback to load the image from a chosen file.
        */
        void loadImage();


        // HSV
        Glib::RefPtr<Gtk::Adjustment> hue_max_adj, hue_min_adj,
                                      sat_max_adj, sat_min_adj,
                                      val_max_adj, val_min_adj;
        bool hsv_blocked = false,
             hue_blocked = false,
             sat_blocked = false,
             val_blocked = false;


        // state tracking
        enum Pages {
            HSV = 0,
            CHANNELS = 1
        };
        image_proc::ModifierOption  current_channel_modifier;
        image_proc::ChannelOption   current_channel_option;
        image_proc::CompressionMode current_compression_mode;
        guint                       current_page_number;


        // Gtk widgets to keep track of
        Gtk::Paned base, left_base;
        Gtk::Switch hv_switch;


        // image handling
        Gtk::Box   images_box;        
        Gtk::Image original_image_widget, altered_image_widget;
        cv::Mat    original_image,        altered_image;
};