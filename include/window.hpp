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
        /* #region      signal handlers */
        /* #region          radio button handlers */
        /**
         * Callback for a change in the compression level.
        */
        void compressionModechange();

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
        /* #endregion       radio button handlers */

        /* #region          button handlers */
        /**
         * Callback for a change in the image orientation.
         * This changed if the images are besides or on top of each other.
         * 
         * @param <unused>
        */
        void orientationChange(const Gtk::StateFlags&);

        /**
         * Callback for a change in editing notebook tab.
         * 
         * @param <unsused>
         * @param page_number: number of the current tab/page
        */
        void switchEditingMode(Gtk::Widget*, guint page_number);

        /**
         * Callback for the LIMIT blocking toggle.
         * This toggle allows for changed in the LIMIT adjustments without triggering a constant reapplication to the image.
         * 
         * @param <unused>
        */
        void directActivationBlockingChanged(const Gtk::StateFlags&);
        /* #endregion       button handlers */

        /* #region          other */
        /**
         * Callback to be called once when the window is first idling, ergo when Gtk completely finished its setup.
         * This method will set the paned windows to reasonable positions.
        */
        void windowFinishSetup();

        /**
         * Callback to synchronize the minimum and maximum adjustment
         * slider values in order to avoid a min > max case.
         * 
         * @param channel_idx: channel on which the change happened
         * @param called_from_min: wether or not this callback is bound to a minimum adjustment or not
        */
        void changedAdjustment(size_t channel_idx, bool called_from_min);
        /* #endregion       other */
        /* #endregion   signal handlers */

        /* #region      apply functions */
        /**
         * Callback to apply the changes made in LIMIT tab.
         * Also applies compression.
        */
        void applyHSVEdits();

        /**
         * Callback to apply the changed made in Channels tab.
         * Also applies compression.
        */
        void applyChannelEdits();
        /* #endregion   apply functions */

        /* #region      image load/save */
        /**
         * Callback to save the image into a chosen location.
        */
        void saveImage();

        /**
         * Callback to load the image from a chosen file.
        */
        void loadImage();

        /**
         * Set preview images for color space.
        */
        void setPreviews();
        /* #endregion   image load/save */

        /* #region      members */
        /* #region          limits  */
        image_proc::ColorSpace      current_color_space = image_proc::ColorSpace::HSV; //only temp
        
        // min and max adjustments for each channel
        // pattern: min, max, min, max, min, max
        std::array<Glib::RefPtr<Gtk::Adjustment>, 2 * NR_CHANNELS> adjustments;
        /**
         * bit 0: channel 0
         * bit 1: channel 1
         * bit 2: channel 2
         * bit 3-7: unused
        */
        uint8_t channel_blocked_flags = 0u;
        bool direct_activation_blocked = false;
        
        // preview handling
        const std::array<const cv::Mat, NR_CHANNELS> limit_preview_matrix_originals = {
            cv::imread("../resources/channel_0.bmp"),
            cv::imread("../resources/channel_1.bmp"),
            cv::imread("../resources/channel_2.bmp"),
        };
        std::array<cv::Mat, NR_CHANNELS>    limit_preview_matrix_references = {
            cv::Mat(255, 5, CV_8UC3),
            cv::Mat(255, 5, CV_8UC3),
            cv::Mat(255, 5, CV_8UC3),
        };
        std::array<Gtk::Image, NR_CHANNELS> limit_preview_images;

        Gtk::Switch direct_application_switch;
        /* #endregion       limits */

        /* #region          channels */
        image_proc::ModifierOption  current_channel_modifier;
        image_proc::ChannelOption   current_channel_option;
        /* #endregion       channels */

        /* #region          general tracking */
        // state tracking
        enum Pages {
            LIMIT = 0,
            CHANNELS = 1
        };
        guint   current_page_number;
        double  current_compression_level = 8.0;

        // Gtk widgets to keep track of
        Gtk::Paned base, left_base;
        Glib::RefPtr<Gtk::Adjustment> compression_level_adj;
        /* #endregion       general tracking*/

        /* #region          image side */
        // Gtk widgets to keep track of
        Gtk::Switch hv_switch;
        Gtk::Label average_label;

        Gtk::Box   images_box;
        Gtk::Image original_image_widget, altered_image_widget;
        cv::Mat    original_image,        altered_image;
        /* #endregion       image side*/
        /* #endregion   members*/
};