#pragma once

#include <gtkmm.h>
#include <array>

#include "image_proc.hpp"

class Window: public Gtk::Window {
    public:
        /**
         * Initialize the main window by setting up the widgets.
        */
        Window();
        ~Window() noexcept {}
        
        
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
         * Callback to change hue adjustments so that min <= max.
         * 
         * @param is_max: wether max was changed, false indicates that min was changed
        */
        void changeHueAdjustment(bool is_max);

        /**
         * Callback to change hue adjustments so that min <= max.
         * 
         * @param is_max: wether max was changed, false indicates that min was changed
        */
        void changeSaturationAdjustment(bool is_max);

        /**
         * Callback to change hue adjustments so that min <= max.
         * 
         * @param is_max: wether max was changed, false indicates that min was changed
        */
        void changeValueAdjustment(bool is_max);

        /**
         * Callback for a change in the compression level.
        */
        void compressionModechange();

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
        void hsvBlockChanged(const Gtk::StateFlags&);

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
        guint                       current_page_number;
        double                      current_compression_level = 8.0;


        // Gtk widgets to keep track of
        Gtk::Paned base, left_base;
        Gtk::Switch hv_switch, hsv_switch;
        Gtk::Label average_label;
        Glib::RefPtr<Gtk::Adjustment> compression_level_adj;

        std::array<cv::Mat, NR_CHANNELS> channel_preview_matrices = {
            cv::Mat(1, 255, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0)),
            cv::Mat(1, 255, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0)),
            cv::Mat(1, 255, CV_8UC3, cv::Scalar(0.0, 0.0, 0.0)),
        };
        std::array<Glib::RefPtr<Gdk::Pixbuf>, NR_CHANNELS> channel_preview_buffers;

        // image handling
        Gtk::Box   images_box;        
        Gtk::Image original_image_widget, altered_image_widget;
        cv::Mat    original_image,        altered_image;
};