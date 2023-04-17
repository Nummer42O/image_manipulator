#include "window.hpp"

#include <iostream>
#include <string>

#define CREATE_MIN_ADJUSTMENT Gtk::Adjustment::create(0.0, 0.0, 255.0)
#define CREATE_MAX_ADJUSTMENT Gtk::Adjustment::create(255.0, 0.0, 255.0)

#define SPACING         5
#define SCALE_PADDING   20

/** TODO:
 *  - add open/save/print shortcut key binds
 *  - add zooming functionality
 *  - add printing
 *  - add hue-, value-, saturation-scale images to show what value is what
 *  - add zoom functionality
 *  - look into hv_switch again, maybe another version? single button?
 *  - HSV presets
 *  - fix windowFinishSetup for moving?
 *  - make compression als scale with 8, 4, 2, 1bit ticks?
*/

Window::Window() {
    this->set_title("Image Manipulator");

    this->base.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    this->base.set_wide_handle();
    this->add(base);

    /* #region      config side (left) */
    this->left_base.set_orientation(Gtk::ORIENTATION_VERTICAL);
    this->base.pack1(left_base, Gtk::EXPAND | Gtk::FILL);

    /* #region          notebook */
    Gtk::Notebook* editing_notebook = Gtk::make_managed<Gtk::Notebook>();
    editing_notebook->signal_switch_page().connect(sigc::mem_fun2(*this, &Window::switchEditingMode));
    this->left_base.pack1(*editing_notebook, Gtk::EXPAND | Gtk::FILL);

    /* #region              HSV manipulation */
    Gtk::Box* hsv_adjustments = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    hsv_adjustments->set_border_width(5);
    editing_notebook->append_page(*hsv_adjustments, "_HSV", true);

    /* #region                  hue */
    Gtk::Frame* hue_frame = Gtk::make_managed<Gtk::Frame>(" Hue ");
    hsv_adjustments->pack_start(*hue_frame, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* hue_adjustment = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    hue_adjustment->set_border_width(SPACING);
    hue_frame->add(*hue_adjustment);
    
    // min
    this->hue_min_adj = CREATE_MIN_ADJUSTMENT;
    this->hue_min_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeHueAdjustment), false));
    Gtk::Scale* hue_min = Gtk::make_managed<Gtk::Scale>(this->hue_min_adj, Gtk::ORIENTATION_VERTICAL);
    hue_min->set_inverted();
    hue_adjustment->pack_start(*hue_min, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);

    // max
    this->hue_max_adj = CREATE_MAX_ADJUSTMENT;
    this->hue_max_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeHueAdjustment), true));
    Gtk::Scale* hue_max = Gtk::make_managed<Gtk::Scale>(this->hue_max_adj, Gtk::ORIENTATION_VERTICAL);
    hue_max->set_inverted();
    hue_adjustment->pack_start(*hue_max, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);
    /* #endregion               hue */

    /* #region                  saturation */
    Gtk::Frame* sat_frame = Gtk::make_managed<Gtk::Frame>(" Saturation ");
    hsv_adjustments->pack_start(*sat_frame, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* sat_adjustment = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    sat_adjustment->set_border_width(SPACING);
    sat_frame->add(*sat_adjustment);
    
    // min
    this->sat_min_adj = CREATE_MIN_ADJUSTMENT;
    this->sat_min_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeSaturationAdjustment), false));
    Gtk::Scale* sat_min = Gtk::make_managed<Gtk::Scale>(this->sat_min_adj, Gtk::ORIENTATION_VERTICAL);
    sat_min->set_inverted();
    sat_adjustment->pack_start(*sat_min, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);

    // max
    this->sat_max_adj = CREATE_MAX_ADJUSTMENT;
    this->sat_max_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeSaturationAdjustment), true));
    Gtk::Scale* sat_max = Gtk::make_managed<Gtk::Scale>(this->sat_max_adj, Gtk::ORIENTATION_VERTICAL);
    sat_max->set_inverted();
    sat_adjustment->pack_start(*sat_max, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);
    /* #endregion               saturation */

    /* #region                  value */
    Gtk::Frame* val_frame = Gtk::make_managed<Gtk::Frame>(" Value ");
    hsv_adjustments->pack_start(*val_frame, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* val_adjustment = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    val_adjustment->set_border_width(SPACING);
    val_frame->add(*val_adjustment);
    
    // min
    this->val_min_adj = CREATE_MIN_ADJUSTMENT;
    this->val_min_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeValueAdjustment), false));
    Gtk::Scale* val_min = Gtk::make_managed<Gtk::Scale>(this->val_min_adj, Gtk::ORIENTATION_VERTICAL);
    val_min->set_inverted();
    val_adjustment->pack_start(*val_min, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);

    // max
    this->val_max_adj = CREATE_MAX_ADJUSTMENT;
    this->val_max_adj->signal_value_changed().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeValueAdjustment), true));
    Gtk::Scale* val_max = Gtk::make_managed<Gtk::Scale>(this->val_max_adj, Gtk::ORIENTATION_VERTICAL);
    val_max->set_inverted();
    val_adjustment->pack_start(*val_max, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);
    /* #endregion               value */

    /* #region                  block hsv adjustment */
    Gtk::Box* blocking_adjustment = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    blocking_adjustment->set_border_width(SPACING);
    hsv_adjustments->pack_end(*blocking_adjustment, Gtk::PACK_SHRINK);

    Gtk::Label* blocking_label = Gtk::make_managed<Gtk::Label>("Block direct processing:");
    blocking_label->set_halign(Gtk::ALIGN_START);
    blocking_adjustment->pack_start(*blocking_label, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Switch* blocking_switch = Gtk::make_managed<Gtk::Switch>();
    blocking_switch->signal_state_flags_changed().connect(sigc::mem_fun1(*this, &Window::hsvBlockChanged));
    blocking_adjustment->pack_end(*blocking_switch, Gtk::PACK_SHRINK);
    /* #endregion               block hsv adjustment*/
    /* #endregion           HSV manipulation */

    /* #region              channel manipulation */
    /* #region                  channel modifiers */
    Gtk::Box* channel_manipulations = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    channel_manipulations->set_border_width(SPACING);
    editing_notebook->append_page(*channel_manipulations, "_Channels", true);

    channel_manipulations->pack_start(*Gtk::make_managed<Gtk::Label>("Choose channel:", Gtk::ALIGN_START), Gtk::PACK_SHRINK);

    Gtk::Box* channel_modifiers_options_horizontal_align = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    channel_manipulations->pack_start(*channel_modifiers_options_horizontal_align, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* channel_modifiers_options_vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    channel_modifiers_options_horizontal_align->pack_start(*channel_modifiers_options_vertical_box, Gtk::PACK_EXPAND_PADDING);

    Gtk::RadioButtonGroup channel_modifier_options;

    /* #region                      mathematic approach */
    // minimum
    Gtk::RadioButton* min_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Minimum");
    min_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::MIN));
    channel_modifiers_options_vertical_box->pack_start(*min_option, Gtk::PACK_EXPAND_PADDING);

    // average
    Gtk::RadioButton* avg_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Average");
    avg_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::AVG));
    avg_option->set_active();
    channel_modifiers_options_vertical_box->pack_start(*avg_option, Gtk::PACK_EXPAND_PADDING);
    
    // maximum
    Gtk::RadioButton* max_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Maximum");
    max_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::MAX));
    channel_modifiers_options_vertical_box->pack_start(*max_option, Gtk::PACK_EXPAND_PADDING);
    /* #endregion                   mathematic approach */

    /* #region                      RGB approach */
    // red
    Gtk::RadioButton* red_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Red");
    red_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::RED));
    channel_modifiers_options_vertical_box->pack_start(*red_option, Gtk::PACK_EXPAND_PADDING);

    // green
    Gtk::RadioButton* green_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Green");
    green_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::GREEN));
    channel_modifiers_options_vertical_box->pack_start(*green_option, Gtk::PACK_EXPAND_PADDING);

    // blue
    Gtk::RadioButton* blue_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Blue");
    blue_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::BLUE));
    channel_modifiers_options_vertical_box->pack_start(*blue_option, Gtk::PACK_EXPAND_PADDING);
    /* #endregion                   RGB approach */

    /* #region                      HSV approach */
    // hue
    Gtk::RadioButton* hue_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Hue");
    hue_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::HUE));
    channel_modifiers_options_vertical_box->pack_start(*hue_option, Gtk::PACK_EXPAND_PADDING);

    // saturation
    Gtk::RadioButton* sat_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Saturation");
    sat_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::SAT));
    channel_modifiers_options_vertical_box->pack_start(*sat_option, Gtk::PACK_EXPAND_PADDING);

    // value
    Gtk::RadioButton* val_option = Gtk::make_managed<Gtk::RadioButton>(channel_modifier_options, "Value");
    val_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorModifier), image_proc::ModifierOption::VAL));
    channel_modifiers_options_vertical_box->pack_start(*val_option, Gtk::PACK_EXPAND_PADDING);
    /* #endregion                   HSV approach */
    /* #endregion               channel modifiers */

    // separator :D
    channel_manipulations->pack_start(*Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL), Gtk::PACK_SHRINK);

    /* #region                  channel options */
    channel_manipulations->pack_start(*Gtk::make_managed<Gtk::Label>("As channel:", Gtk::ALIGN_START), Gtk::PACK_SHRINK);

    Gtk::Box* channel_options_horizontal_align = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    channel_manipulations->pack_start(*channel_options_horizontal_align, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* channel_options_vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    channel_options_horizontal_align->pack_start(*channel_options_vertical_box, Gtk::PACK_EXPAND_PADDING);

    Gtk::RadioButtonGroup channel_options;

    // red
    Gtk::RadioButton* red_channel_option = Gtk::make_managed<Gtk::RadioButton>(channel_options, "Red");
    red_channel_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorChannel), image_proc::ChannelOption::R));
    channel_options_vertical_box->pack_start(*red_channel_option, Gtk::PACK_EXPAND_PADDING);

    // green
    Gtk::RadioButton* green_channel_option = Gtk::make_managed<Gtk::RadioButton>(channel_options, "Green");
    green_channel_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorChannel), image_proc::ChannelOption::G));
    channel_options_vertical_box->pack_start(*green_channel_option, Gtk::PACK_EXPAND_PADDING);

    // blue
    Gtk::RadioButton* blue_channel_option = Gtk::make_managed<Gtk::RadioButton>(channel_options, "Blue");
    blue_channel_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorChannel), image_proc::ChannelOption::B));
    channel_options_vertical_box->pack_start(*blue_channel_option, Gtk::PACK_EXPAND_PADDING);

    // all
    Gtk::RadioButton* all_channel_option = Gtk::make_managed<Gtk::RadioButton>(channel_options, "All");
    all_channel_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::changeChannelManipulatorChannel), image_proc::ChannelOption::ALL));
    all_channel_option->set_active();
    channel_options_vertical_box->pack_start(*all_channel_option, Gtk::PACK_EXPAND_PADDING);
    /* #endregion               channel options */
    /* #endregion           channel manipulation */
    /* #endregion       notebook */

    /* #region          compression options */
    Gtk::Box* compression_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    compression_box->set_border_width(SPACING);
    this->left_base.pack2(*compression_box, Gtk::EXPAND | Gtk::FILL);

    compression_box->pack_start(*Gtk::make_managed<Gtk::Label>("Compression modes:"), Gtk::PACK_SHRINK);

    Gtk::Box* compression_modes_horizontal_align = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    compression_box->pack_start(*compression_modes_horizontal_align, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* compression_modes_vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    compression_modes_horizontal_align->pack_start(*compression_modes_vertical_box, Gtk::PACK_EXPAND_PADDING);

    Gtk::RadioButtonGroup compression_mode_group;

    /* #region              options */
    // 1 bit option
    Gtk::RadioButton* one_bit_option = Gtk::make_managed<Gtk::RadioButton>(compression_mode_group, "1 bit");
    one_bit_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::compressionModechange), image_proc::CompressionMode::ONE_BIT));
    compression_modes_vertical_box->pack_start(*one_bit_option, Gtk::PACK_EXPAND_PADDING);
    
    // 2 bit option
    Gtk::RadioButton* two_bit_option = Gtk::make_managed<Gtk::RadioButton>(compression_mode_group, "2 bit");
    two_bit_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::compressionModechange), image_proc::CompressionMode::TWO_BIT));
    compression_modes_vertical_box->pack_start(*two_bit_option, Gtk::PACK_EXPAND_PADDING);

    // 4 bit option
    Gtk::RadioButton* four_bit_option = Gtk::make_managed<Gtk::RadioButton>(compression_mode_group, "4 bit");
    four_bit_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::compressionModechange), image_proc::CompressionMode::FOUR_BIT));
    compression_modes_vertical_box->pack_start(*four_bit_option, Gtk::PACK_EXPAND_PADDING);

    // 8 bit / normal option
    Gtk::RadioButton* normal_option = Gtk::make_managed<Gtk::RadioButton>(compression_mode_group, "8 bit");
    normal_option->signal_clicked().connect(sigc::bind(sigc::mem_fun1(*this, &Window::compressionModechange), image_proc::CompressionMode::NORMAL));
    normal_option->set_active();
    compression_modes_vertical_box->pack_start(*normal_option, Gtk::PACK_EXPAND_PADDING);
    /* #endregion           options*/
    /* #endregion       compression options */
    /* #endregion   config side (left)*/

    /* #region      image side (right) */
    Gtk::Box* right_base = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    this->base.pack2(*right_base, Gtk::EXPAND | Gtk::FILL);

    /* #region          utility bar */
    /* #region              buttons */
    Gtk::Box* utility_bar = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    utility_bar->set_border_width(SPACING);
    right_base->pack_start(*utility_bar, Gtk::PACK_SHRINK);

    // load image button
    //TODO: icon?
    Gtk::Button* load_button = Gtk::make_managed<Gtk::Button>("_Load", true);
    load_button->signal_clicked().connect(sigc::mem_fun0(*this, &Window::loadImage));
    utility_bar->pack_start(*load_button, Gtk::PACK_SHRINK);

    // save image button
    //TODO: icon?
    Gtk::Button* save_button = Gtk::make_managed<Gtk::Button>("_Save", true);
    save_button->signal_clicked().connect(sigc::mem_fun0(*this, &Window::saveImage));
    utility_bar->pack_start(*save_button, Gtk::PACK_SHRINK);

    // print
    //TODO
    /* #endregion           buttons */

    utility_bar->pack_start(this->average_label, Gtk::PACK_EXPAND_PADDING);

    /* #region              horizontal vertical switch */
    Gtk::Box* hv_switch_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    utility_bar->pack_end(*hv_switch_box, Gtk::PACK_SHRINK);

    Gtk::Image* horizontal_option = Gtk::make_managed<Gtk::Image>("object-flip-horizontal", Gtk::ICON_SIZE_LARGE_TOOLBAR);
    hv_switch_box->pack_start(*horizontal_option, Gtk::PACK_SHRINK);

    Gtk::Box* vertical_align_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    hv_switch_box->pack_start(*vertical_align_box, Gtk::PACK_SHRINK);
    this->hv_switch.signal_state_flags_changed().connect(sigc::mem_fun1(*this, &Window::orientationChange));
    vertical_align_box->pack_start(this->hv_switch, Gtk::PACK_SHRINK);
    
    Gtk::Image* vertical_option = Gtk::make_managed<Gtk::Image>("object-flip-vertical", Gtk::ICON_SIZE_LARGE_TOOLBAR);
    hv_switch_box->pack_start(*vertical_option, Gtk::PACK_SHRINK);
    /* #endregion           horizontal vertical switch */

    // visual separation from image area
    right_base->pack_start(*Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL), Gtk::PACK_SHRINK);
    /* #endregion       utility bar */

    /* #region          images */
    Gtk::ScrolledWindow* image_scroll_window = Gtk::make_managed<Gtk::ScrolledWindow>();
    image_scroll_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    right_base->pack_end(*image_scroll_window, Gtk::PACK_EXPAND_WIDGET);

    this->images_box.set_border_width(5);
    image_scroll_window->add(this->images_box);

    this->images_box.pack_start(this->original_image_widget, Gtk::PACK_EXPAND_PADDING);
    this->images_box.pack_end(this->altered_image_widget, Gtk::PACK_EXPAND_PADDING);
    /* #endregion       images */
    /* #endregion   image side (right) */

    Glib::signal_idle().connect_once(sigc::mem_fun0(*this, &Window::windowFinishSetup));

    // show
    this->maximize();
    this->show_all_children();
}

void Window::loadImage(const std::string& filepath) {
    if (filepath.empty()) {
        return;
    }

    // load initial image
    if (!image_proc::loadImage(this->original_image, filepath)) {
        Gtk::MessageDialog dialog(*this, "Failed to load initial image:", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.set_secondary_text(filepath);
        dialog.run();
    } else {
        Window::convertCVtoGTK(this->original_image, this->original_image_widget);

        if (this->current_page_number == Pages::HSV) {
            if (this->hsv_blocked) {
                return;
            }

            this->applyHSVEdits();
        } else {
            this->applyChannelEdits();
        }
    }
}

void Window::convertCVtoGTK(const cv::Mat& src, Gtk::Image& dst) {
    assert(src.data != NULL);

    Glib::RefPtr<Gdk::Pixbuf> image_buffer = Gdk::Pixbuf::create_from_data(src.data, Gdk::COLORSPACE_RGB, false, 8, src.cols, src.rows, src.step);

    dst.set(image_buffer);
}

void Window::changeHueAdjustment(bool is_max) {
    if (!this->hue_blocked) {
        this->hue_blocked = true;

        double max_value = this->hue_max_adj->get_value(),
               min_value = this->hue_min_adj->get_value();

        if (is_max && min_value > max_value) {
            this->hue_min_adj->set_value(max_value);
        } else if (!is_max && max_value < min_value) {
            this->hue_max_adj->set_value(min_value);
        }

        if (!this->hsv_blocked) {
            this->applyHSVEdits();
        }

        this->hue_blocked = false;
    }
}

void Window::changeSaturationAdjustment(bool is_max) {
    if (!this->sat_blocked) {
        this->sat_blocked = true;

        double max_value = this->sat_max_adj->get_value(),
               min_value = this->sat_min_adj->get_value();

        if (is_max && min_value > max_value) {
            this->sat_min_adj->set_value(max_value);
        } else if (!is_max && max_value < min_value) {
            this->sat_max_adj->set_value(min_value);
        }

        if (!this->hsv_blocked) {
            this->applyHSVEdits();
        }

        this->sat_blocked = false;
    }
}

void Window::changeValueAdjustment(bool is_max) {
    if (!this->val_blocked) {
        this->val_blocked = true;

        double max_value = this->val_max_adj->get_value(),
               min_value = this->val_min_adj->get_value();

        if (is_max && min_value > max_value) {
            this->val_min_adj->set_value(max_value);
        } else if (!is_max && max_value < min_value) {
            this->val_max_adj->set_value(min_value);
        }

        if (!this->hsv_blocked) {
            this->applyHSVEdits();
        }

        this->val_blocked = false;
    }
}

void Window::compressionModechange(const image_proc::CompressionMode& mode) {
    this->current_compression_mode = mode;

    if (this->current_page_number == Pages::HSV) {
        if (this->hsv_blocked) {
            return;
        }

        this->applyHSVEdits();
    } else {
        this->applyChannelEdits();
    }
}

void Window::orientationChange(const Gtk::StateFlags&) {
    this->images_box.set_orientation(this->hv_switch.get_state() ? Gtk::ORIENTATION_VERTICAL : Gtk::ORIENTATION_HORIZONTAL);
}

void Window::changeChannelManipulatorModifier(const image_proc::ModifierOption& option) {
    this->current_channel_modifier = option;

    if (this->current_page_number == Pages::CHANNELS) {
        this->applyChannelEdits();
    }
}

void Window::changeChannelManipulatorChannel(const image_proc::ChannelOption& option) {
    this->current_channel_option = option;

    if (this->current_page_number == Pages::CHANNELS) {
        this->applyChannelEdits();
    }
}

void Window::switchEditingMode(Gtk::Widget*, guint page_number) {
    this->current_page_number = page_number;

    if (this->current_page_number == Pages::HSV) {
        if (this->hsv_blocked) {
            return;
        }

        this->applyHSVEdits();
    } else {
        this->applyChannelEdits();
    }
}

void Window::hsvBlockChanged(const Gtk::StateFlags& flags) {
    this->hsv_blocked = (flags & Gtk::STATE_FLAG_CHECKED);
}

void Window::windowFinishSetup() {
    const int min_position_base      = this->base.property_min_position(),
              max_position_left_base = this->left_base.property_max_position();

    std::clog << "min base pos: " << min_position_base << '\n'
              << "max left pos: " << max_position_left_base << std::endl;

    this->base.set_position(min_position_base);
    this->left_base.set_position(max_position_left_base);
}

void Window::applyHSVEdits() {
    if (this->original_image.empty()) {
        return;
    }

    double hue_min = this->hue_min_adj->get_value(),
           hue_max = this->hue_max_adj->get_value(),
           sat_min = this->sat_min_adj->get_value(),
           sat_max = this->sat_max_adj->get_value(),
           val_min = this->val_min_adj->get_value(),
           val_max = this->val_max_adj->get_value();

    cv::Mat temp;
    image_proc::limitImageByHSV(this->original_image, temp,
                                hue_min, hue_max, sat_min, sat_max, val_min, val_max);
    image_proc::compressImage(temp, this->altered_image, this->current_compression_mode);

    this->average_label.set_text(image_proc::getAverageColorString(this->altered_image));
    
    Window::convertCVtoGTK(this->altered_image, this->altered_image_widget);
}

void Window::applyChannelEdits() {
    if (this->original_image.empty()) {
        return;
    }

    cv::Mat temp;
    image_proc::manipulateChannels(this->original_image, temp, this->current_channel_modifier, this->current_channel_option);
    image_proc::compressImage(temp, this->altered_image, this->current_compression_mode);

    this->average_label.set_text(image_proc::getAverageColorString(this->altered_image));

    Window::convertCVtoGTK(this->altered_image, this->altered_image_widget);
}

void Window::saveImage() {
    // sanity check
    assert(!this->original_image.empty());

    Gtk::FileChooserDialog dialog(*this, "Save", Gtk::FILE_CHOOSER_ACTION_SAVE, Gtk::DIALOG_DESTROY_WITH_PARENT & Gtk::DIALOG_MODAL);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
    filter->set_name("Images");
    filter->add_mime_type("image/png");
    filter->add_pattern("*.png");
    filter->add_mime_type("image/jpeg");
    filter->add_pattern("*.jpeg");
    filter->add_pattern("*.jpg");
    filter->add_mime_type("image/gif");
    filter->add_pattern("*.gif");
    dialog.add_filter(filter);
    
    Glib::RefPtr<Gtk::FileFilter> extra = Gtk::FileFilter::create();
    extra->set_name("All files");
    extra->add_pattern("*.*");
    dialog.add_filter(extra);

    std::string filepath;
    int response = dialog.run();
    switch (response) {
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_DELETE_EVENT:
            return;
        case Gtk::RESPONSE_OK:
            filepath = dialog.get_filename();
            break;
        default:
            std::cerr << "ERROR: unexpected response " << response << '.' << std::endl;
            exit(1);
    }

    if (!image_proc::saveImage(this->altered_image, filepath)) {
        Gtk::MessageDialog dialog(*this, "Failed to save image.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
}

void Window::loadImage() {
    Gtk::FileChooserDialog dialog(*this, "Open", Gtk::FILE_CHOOSER_ACTION_SAVE, Gtk::DIALOG_DESTROY_WITH_PARENT & Gtk::DIALOG_MODAL);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
    filter->set_name("Images");
    filter->add_mime_type("image/png");
    filter->add_pattern("*.png");
    filter->add_mime_type("image/jpeg");
    filter->add_pattern("*.jpeg");
    filter->add_pattern("*.jpg");
    filter->add_mime_type("image/gif");
    filter->add_pattern("*.gif");
    dialog.add_filter(filter);
    
    Glib::RefPtr<Gtk::FileFilter> extra = Gtk::FileFilter::create();
    extra->set_name("All files");
    extra->add_pattern("*.*");
    dialog.add_filter(extra);

    std::string filepath;
    int response = dialog.run();
    switch (response) {
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_DELETE_EVENT:
            return;
        case Gtk::RESPONSE_OK:
            filepath = dialog.get_filename();
            break;
        default:
            std::cerr << "ERROR: unexpected response " << response << '.' << std::endl;
            exit(1);
    }

    if (!image_proc::loadImage(this->original_image, filepath)) {
        Gtk::MessageDialog dialog(*this, "Failed to load image:", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.set_secondary_text(filepath);
        dialog.run();
    } else {
        Window::convertCVtoGTK(this->original_image, this->original_image_widget);

        if (this->current_page_number == Pages::HSV) {
            if (this->hsv_blocked) {
                return;
            }

            this->applyHSVEdits();
        } else {
            this->applyChannelEdits();
        }
    }
}