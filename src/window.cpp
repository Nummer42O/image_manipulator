#include <iostream>
#include <string>

#include "window.hpp"


#define CREATE_MIN_ADJUSTMENT Gtk::Adjustment::create(0.0, 0.0, 255.0)
#define CREATE_MAX_ADJUSTMENT Gtk::Adjustment::create(255.0, 0.0, 255.0)

#define SPACING         5
#define SCALE_PADDING   5


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

    // initialization of color space data model
    this->color_space_data = Gtk::ListStore::create(this->color_space_data_columns);

    Gtk::TreeModel::iterator current_limit_color_space_iter;
    for (size_t i = 0ul; i < image_proc::ColorSpace::LAST; i++) {
        Gtk::TreeModel::Row row;
        if (i == this->current_limit_color_space) {
            current_limit_color_space_iter = this->color_space_data->append();
            row = *current_limit_color_space_iter;
        } else {
            row = *(this->color_space_data->append());
        }

        row[this->color_space_data_columns.color_space]       = static_cast<image_proc::ColorSpace>(i);
        row[this->color_space_data_columns.color_space_name]  = image_proc::color_space_names[i];
    }

    /* #region              LIMIT manipulation */
    Gtk::Box* limit_adjustments = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    limit_adjustments->set_border_width(5);
    editing_notebook->append_page(*limit_adjustments, "_Limits", true);

    /* #region                  color space selection */
    Gtk::Box* color_space_selector_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    color_space_selector_box->set_border_width(SPACING);
    limit_adjustments->pack_start(*color_space_selector_box, Gtk::PACK_SHRINK);

    Gtk::Label* color_space_selector_label = Gtk::make_managed<Gtk::Label>("Color Space:");
    color_space_selector_label->set_halign(Gtk::ALIGN_START);
    color_space_selector_box->pack_start(*color_space_selector_label, Gtk::PACK_SHRINK);

    this->limit_color_space_selector.set_model(this->color_space_data);
    this->limit_color_space_selector.pack_start(this->color_space_data_columns.color_space_name);
    this->limit_color_space_selector.set_active(current_limit_color_space_iter);
    this->limit_color_space_selector.signal_changed().connect(sigc::mem_fun0(*this, &Window::limitColorSpaceChanged));
    color_space_selector_box->pack_end(this->limit_color_space_selector, Gtk::PACK_EXPAND_WIDGET);
    /* #endregion               color space selection */

    this->getPreviews();
    for (size_t i = 0; i < NR_CHANNELS; i++) {
        this->limit_channel_frames[i] = Gtk::Frame(image_proc::color_space_channels[this->current_limit_color_space][i]);
        limit_adjustments->pack_start(this->limit_channel_frames[i], Gtk::PACK_EXPAND_WIDGET);

        Gtk::Box* adjustments_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
        adjustments_box->set_border_width(SPACING);
        this->limit_channel_frames[i].add(*adjustments_box);
        
        // min
        size_t adjustments_idx = i * 2ul;
        this->limit_adjustments[adjustments_idx] = CREATE_MIN_ADJUSTMENT;
        this->limit_adjustments[adjustments_idx]->signal_value_changed().connect(sigc::bind(sigc::mem_fun2(*this, &Window::changedAdjustment), i, true));
        this->limit_min_scales[i] = Gtk::Scale(this->limit_adjustments[adjustments_idx], Gtk::ORIENTATION_VERTICAL);
        this->limit_min_scales[i].set_inverted();
        adjustments_box->pack_start(this->limit_min_scales[i], Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);

        // preview
        Gtk::ScrolledWindow* limit_preview_scaling = Gtk::make_managed<Gtk::ScrolledWindow>();
        limit_preview_scaling->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        limit_preview_scaling->signal_size_allocate().connect(sigc::bind(sigc::mem_fun2(*this, &Window::limitPreviewChangedSize), i));
        adjustments_box->pack_start(*limit_preview_scaling, Gtk::PACK_SHRINK);
        
        image_proc::convertCVtoGTK(this->limit_preview_references[this->current_limit_color_space][i], this->limit_preview_images[i]);
        limit_preview_scaling->add(this->limit_preview_images[i]);

        // max
        adjustments_idx++;
        this->limit_adjustments[adjustments_idx] = CREATE_MAX_ADJUSTMENT;
        this->limit_adjustments[adjustments_idx]->signal_value_changed().connect(sigc::bind(sigc::mem_fun2(*this, &Window::changedAdjustment), i, false));
        Gtk::Scale* max_scale = Gtk::make_managed<Gtk::Scale>(this->limit_adjustments[adjustments_idx], Gtk::ORIENTATION_VERTICAL);
        max_scale->set_inverted();
        adjustments_box->pack_start(*max_scale, Gtk::PACK_EXPAND_PADDING, SCALE_PADDING);
    }

    /* #region                  block hsv adjustment */
    Gtk::Box* blocking_adjustment = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, SPACING);
    blocking_adjustment->set_border_width(SPACING);
    limit_adjustments->pack_end(*blocking_adjustment, Gtk::PACK_SHRINK);

    Gtk::Label* blocking_label = Gtk::make_managed<Gtk::Label>("Block direct processing:");
    blocking_label->set_halign(Gtk::ALIGN_START);
    blocking_adjustment->pack_start(*blocking_label, Gtk::PACK_EXPAND_WIDGET);

    this->direct_application_switch.signal_state_flags_changed().connect(sigc::mem_fun1(*this, &Window::directActivationBlockingChanged));
    blocking_adjustment->pack_end(this->direct_application_switch, Gtk::PACK_SHRINK);
    /* #endregion               block hsv adjustment*/
    /* #endregion           LIMIT manipulation */

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

    /* #region          compression  */
    Gtk::Box* compression_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, SPACING);
    compression_box->set_border_width(SPACING);
    this->left_base.pack2(*compression_box, Gtk::EXPAND | Gtk::FILL);

    compression_box->pack_start(*Gtk::make_managed<Gtk::Label>("Compression modes:"), Gtk::PACK_SHRINK);

    Gtk::Box* compression_modes_horizontal_align = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    compression_box->pack_start(*compression_modes_horizontal_align, Gtk::PACK_EXPAND_WIDGET);

    this->compression_level_adj = Gtk::Adjustment::create(8.0, 1.0, 8.0, .1, 1.0);
    this->compression_level_adj->signal_value_changed().connect(sigc::mem_fun0(*this, &Window::compressionModechange));
    Gtk::Scale* compression_level = Gtk::make_managed<Gtk::Scale>(this->compression_level_adj, Gtk::ORIENTATION_VERTICAL);
    compression_level->set_has_origin(false);
    compression_level->set_digits(2);
    compression_level->set_value_pos(Gtk::POS_LEFT);
    compression_level->add_mark(1.0, Gtk::POS_RIGHT, "1.0");
    compression_level->add_mark(2.0, Gtk::POS_RIGHT, "2.0");
    compression_level->add_mark(4.0, Gtk::POS_RIGHT, "4.0");
    compression_level->add_mark(8.0, Gtk::POS_RIGHT, "8.0 (default)");
    compression_level->set_size_request(-1, 200);
    compression_modes_horizontal_align->pack_start(*compression_level, Gtk::PACK_EXPAND_PADDING);
    /* #endregion       compression  */
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
    //TODO: print, icon?
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

/* #region      signal handlers */
/* #region          selection signals */
void Window::compressionModechange() {
    this->current_compression_level = this->compression_level_adj->get_value();

    if (this->current_page_number == Pages::LIMIT) {
        if (this->direct_activation_blocked) {
            return;
        }

        this->applyLimitEdits();
    } else {
        this->applyChannelEdits();
    }
}

void Window::limitColorSpaceChanged() {
    Gtk::TreeModel::iterator color_space_data_iter = this->limit_color_space_selector.get_active();

    // sanity check; should not be able to fail
    assert(color_space_data_iter);
    image_proc::ColorSpace new_color_space = (*color_space_data_iter)[this->color_space_data_columns.color_space];

    if (new_color_space == this->current_limit_color_space) {
        return;
    } else {
        this->current_limit_color_space = new_color_space;
    }

    this->getPreviews();

    Gdk::Rectangle rect;
    for (size_t i = 0ul; i < NR_CHANNELS; i++) {
        image_proc::convertCVtoGTK(this->limit_preview_references[new_color_space][i], this->limit_preview_images[i]);
        this->limitPreviewChangedSize(rect, i);

        this->limit_channel_frames[i].set_label(image_proc::color_space_channels[new_color_space][i]);
    }

    this->applyLimitEdits();
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
/* #endregion       selection signals */

/* #region          button signals */
void Window::orientationChange(const Gtk::StateFlags&) {
    this->images_box.set_orientation(this->hv_switch.get_state() ? Gtk::ORIENTATION_VERTICAL : Gtk::ORIENTATION_HORIZONTAL);
}

void Window::switchEditingMode(Gtk::Widget*, guint page_number) {
    this->current_page_number = page_number;

    if (this->current_page_number == Pages::LIMIT) {
        if (this->direct_activation_blocked) {
            return;
        }

        this->applyLimitEdits();
    } else {
        this->applyChannelEdits();
    }
}

void Window::directActivationBlockingChanged(const Gtk::StateFlags&) {
    this->direct_activation_blocked = this->direct_application_switch.get_state();

    if (this->current_page_number == Pages::LIMIT && !this->direct_activation_blocked) {
        this->applyLimitEdits();
    }
}
/* #endregion       button signals */

/* #region          other */
void Window::windowFinishSetup() {
    const int min_position_base      = this->base.property_min_position(),
              max_position_left_base = this->left_base.property_max_position();

    // std::clog << "min base pos: " << min_position_base << '\n'
    //           << "max left pos: " << max_position_left_base << std::endl;

    this->base.set_position(min_position_base);
    this->left_base.set_position(max_position_left_base);
}

void Window::changedAdjustment(size_t channel_idx, bool called_from_min) {
    uint8_t blocked_mask = 1u << channel_idx;

    // check if no adjustment synchronyzing is already in progress
    if (!(this->channel_blocked_flags & blocked_mask)) {
        // switch blocking for this channel on
        this->channel_blocked_flags ^= blocked_mask;

        size_t adjustments_base_idx = channel_idx * 2ul;
        double min_value = this->limit_adjustments[adjustments_base_idx]->get_value(),
               max_value = this->limit_adjustments[adjustments_base_idx + 1ul]->get_value();
        
        if (max_value < min_value) {
            if (called_from_min) {
                this->limit_adjustments[adjustments_base_idx + 1ul]->set_value(min_value);
            } else {
                this->limit_adjustments[adjustments_base_idx]->set_value(max_value);
            }
        }

        this->applyLimitEdits();

        // switch blocking for this channel off
        this->channel_blocked_flags ^= blocked_mask;
    }
}

void Window::limitPreviewChangedSize(Gtk::Allocation&, const size_t& channel_idx) {
    const Gdk::Rectangle scale_rect = this->limit_min_scales[channel_idx].get_range_rect();

    const int scale_height = scale_rect.get_height(),
              scale_y      = scale_rect.get_y();
    
    int limit_preview_width  = STD_PREVIEW_WIDTH, // scale_height * this->limit_preview_aspect_ratio,
        limit_preview_height = scale_height;

    Glib::RefPtr<Gdk::Pixbuf> buffer = this->limit_preview_images[channel_idx].get_pixbuf();
    if (!buffer) {
        std::clog << "Image for color space: " << image_proc::color_space_names[this->current_limit_color_space]
                  << " channel "               << channel_idx + 1
                  << " not yet set. Skipping." << std::endl;

        return;
    }

    buffer = buffer->scale_simple(limit_preview_width, limit_preview_height, Gdk::INTERP_BILINEAR);
    this->limit_preview_images[channel_idx].set(buffer);

    this->limit_preview_images[channel_idx].set_margin_top(scale_y / 2);
}
/* #endregion       other */
/* #endregion   signal handlers*/

/* #region      apply functions */
void Window::applyLimitEdits() {
    if (this->original_image.empty() || this->direct_activation_blocked) {
        return;
    }

    cv::Mat temp;
    image_proc::limitImageByChannels(this->original_image, temp, this->current_limit_color_space,
                                     this->limit_adjustments[0]->get_value(), this->limit_adjustments[1]->get_value(),
                                     this->limit_adjustments[2]->get_value(), this->limit_adjustments[3]->get_value(),
                                     this->limit_adjustments[4]->get_value(), this->limit_adjustments[5]->get_value());
    image_proc::compressImage(temp, this->altered_image, this->current_compression_level);

    this->average_label.set_text(image_proc::getAverageColorString(this->altered_image));
    
    image_proc::convertCVtoGTK(this->altered_image, this->altered_image_widget);
}

void Window::applyChannelEdits() {
    if (this->original_image.empty()) {
        return;
    }

    cv::Mat temp;
    image_proc::manipulateChannels(this->original_image, temp, this->current_channel_modifier, this->current_channel_option);
    image_proc::compressImage(temp, this->altered_image, this->current_compression_level);

    this->average_label.set_text(image_proc::getAverageColorString(this->altered_image));

    image_proc::convertCVtoGTK(this->altered_image, this->altered_image_widget);
}
/* #endregion   apply functions*/

/* #region      image load/save */
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
        image_proc::convertCVtoGTK(this->original_image, this->original_image_widget);

        if (this->current_page_number == Pages::LIMIT) {
            if (this->direct_activation_blocked) {
                return;
            }

            this->applyLimitEdits();
        } else {
            this->applyChannelEdits();
        }
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
        image_proc::convertCVtoGTK(this->original_image, this->original_image_widget);

        if (this->current_page_number == Pages::LIMIT) {
            if (this->direct_activation_blocked) {
                return;
            }

            this->applyLimitEdits();
        } else {
            this->applyChannelEdits();
        }
    }
}

void Window::getPreviews() {
    if (!this->limit_preview_references[this->current_limit_color_space][0].empty()) {
        return;
    }

    std::clog << "Loading previews for " << image_proc::color_space_names[this->current_limit_color_space] << std::endl;

    cv::Mat loaded_image;
    std::string filepath;

    for (size_t i = 0ul; i < NR_CHANNELS; i++) {
        filepath = "../resources/" + image_proc::color_space_names[this->current_limit_color_space] + '_' + image_proc::color_space_channels[this->current_limit_color_space][i] + ".bmp";
        loaded_image = cv::imread(filepath);
        
        if (loaded_image.empty()) {
            Gtk::MessageDialog dialog(*this, "Failed to load preview image:", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dialog.set_secondary_text(filepath);
            dialog.run();

            this->limit_preview_references[this->current_limit_color_space][i] = this->default_preview_image;
        } else {
            this->limit_preview_references[this->current_limit_color_space][i] = std::move(loaded_image);
        }
    }
}
/* #endregion   image load/save*/
