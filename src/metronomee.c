#include "metronomee.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *box;
	Evas_Object *layout;
	Evas_Object *label;
	Evas_Object *button;
	bool button_showing_play;
	Eext_Circle_Surface *circle_surface;
	Evas_Object *circle_slider;
} appdata_s;

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

/* Callback for the "clicked" signal */
/* Called when the button is clicked by the user */
static void
clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = (appdata_s*)data;
	if(ad->button_showing_play) {
		elm_object_text_set(ad->button, "Stop");
		ad->button_showing_play = false;
	} else {
		elm_object_text_set(ad->button, "Play");
		ad->button_showing_play = true;
	}
}

/* Callback for the "value,changed" signal */
/* Called when the value of the circle slider is changed */
static void
_value_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
    double _val = eext_circle_object_value_get(obj);
    dlog_print(DLOG_INFO, LOG_TAG, "Circle slider value changed. %0.1f\n", _val);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create the box component using the conformant
	 *  as the parent, and set the box as the conformant content: */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Layout */
	ad->layout = elm_layout_add(ad->conform);
	elm_layout_theme_set(ad->layout, "layout", "bottom_button", "default");
	evas_object_show(ad->layout);
	elm_object_content_set(ad->conform, ad->layout);

    /* Circle Slider */
	const double MIN_ANGLE = -120.0;
	const double MAX_ANGLE = 120.0;
    ad->circle_surface = eext_circle_surface_conformant_add(ad->conform);
    eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
    eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);
    ad->circle_slider = eext_circle_object_slider_add(ad->layout, ad->circle_surface);
    eext_circle_object_angle_min_max_set(ad->circle_slider, MIN_ANGLE, MAX_ANGLE);
    eext_circle_object_item_angle_min_max_set(ad->circle_slider, "bg", MIN_ANGLE, MAX_ANGLE);
    eext_circle_object_value_min_max_set(ad->circle_slider, 40.0, 218.0); // range from 40bpm to 218bpm
    eext_circle_object_value_set(ad->circle_slider, 100.0);
    eext_circle_object_slider_step_set(ad->circle_slider, 1.0);
    eext_circle_object_color_set(ad->circle_slider, 0, 0, 255, 255);
    eext_circle_object_item_color_set(ad->circle_slider, "bg",  0, 0, 255, 125); // set alpha to translucent for background bar
    double default_radius = eext_circle_object_radius_get(ad->circle_slider);
    eext_circle_object_radius_set(ad->circle_slider, default_radius * 0.7);
    eext_circle_object_item_radius_set(ad->circle_slider, "bg", default_radius * 0.7);
    eext_rotary_object_event_activated_set(ad->circle_slider, EINA_TRUE);
    evas_object_smart_callback_add(ad->circle_slider, "value,changed", _value_changed_cb, 0);

	/* Bottom button */
	ad->button = elm_button_add(ad->layout);
	elm_object_text_set(ad->button, "Play");
	elm_object_style_set(ad->button, "bottom");
	evas_object_smart_callback_add(ad->button, "clicked", clicked_cb, ad);
	evas_object_show(ad->button);
    elm_object_part_content_set(ad->layout, "elm.swallow.button", ad->button);
    ad->button_showing_play = true;

	/* Box that contains a Label */
	ad->box = elm_box_add(ad->layout);
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(ad->box);
	elm_object_content_set(ad->layout, ad->box);
	ad->label = elm_label_add(ad->box);
	elm_object_text_set(ad->label, "<align=center>100 BPM</align>");
	evas_object_size_hint_weight_set(ad->label, 0.0, 0.0);
	/* Comment out the elm_object_content_set() function */
	/* elm_object_content_set(ad->conform, ad->label); */
	evas_object_size_hint_align_set(ad->label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(ad->label, 50, 50);
	evas_object_show(ad->label);
	elm_box_pack_end(ad->box, ad->label);
	elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->box);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
