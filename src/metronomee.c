#include "metronomee.h"
#include <player.h>
#include <storage.h>
#include <stdlib.h>
#include <string.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Eext_Circle_Surface *circle_surface;
	Evas_Object *circle_slider;
	Evas_Object *layout;
	Evas_Object *button;
	Evas_Object *box;
	Evas_Object *label;
	bool button_showing_play;
	player_h player0;
	player_h player1;
	player_h player2;
	player_h player3;
	Ecore_Timer *timer;
} appdata_s;

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

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

Eina_Bool
my_timed_cb(void *data)
{
	appdata_s *ad = data;
	static int count = 0;
	int remainder = count % 4;
	int error_code = 0;

    if (true) {

    	// play rhythm of 'pling~klack~klack~klack'
    	switch(remainder) {

    	   case 0  :
    		    error_code = player_stop(ad->player0);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to stop player 0: error code = %d", error_code);
				error_code = player_start(ad->player0);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to start player 0: error code = %d", error_code);
				break;

    	   case 1  :
				error_code = player_stop(ad->player1);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to stop player 1: error code = %d", error_code);

				error_code = player_start(ad->player1);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to start player 1: error code = %d", error_code);
				break;

    	   case 2  :
				error_code = player_stop(ad->player2);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to stop player 2: error code = %d", error_code);

				error_code = player_start(ad->player2);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to start player 2: error code = %d", error_code);
				break;

    	   case 3  :
				error_code = player_stop(ad->player3);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to stop player 3: error code = %d", error_code);

				error_code = player_start(ad->player3);
				if (error_code != PLAYER_ERROR_NONE)
					dlog_print(DLOG_ERROR, LOG_TAG, "failed to start player 3: error code = %d", error_code);
				break;
    	}

    	if(count == 100)
    		count = 0;
    	count++;
        return ECORE_CALLBACK_RENEW;
    }

    return ECORE_CALLBACK_CANCEL;
}

/* Callback for the "clicked" signal */
/* Called when the button is clicked by the user */
static void
_bottom_button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	if(ad->button_showing_play) {
		elm_object_text_set(ad->button, "Stop");

		ad->timer = ecore_timer_add(0.375, my_timed_cb, ad);

		ad->button_showing_play = false;
	} else {
		elm_object_text_set(ad->button, "Play");

		ecore_timer_del(ad->timer);

		ad->button_showing_play = true;
	}
}

/* Callback for the "value,changed" signal */
/* Called when the value of the circle slider is changed */
static void
_circle_slider_value_changed_cb(void *data, Evas_Object *circle_slider, void *event_info)
{
	appdata_s *ad = data;
	int bpm = (int) eext_circle_object_value_get(circle_slider);

	char *text = (char*)malloc(29 * sizeof(char));
	sprintf(text, "%s%d %s", "<align=center>", bpm, "BPM</align>");
	elm_object_text_set(ad->label, text);

	free(text);
	text = NULL;
}

static void
init_base_player(appdata_s *ad)
{
	int error_code = 0;

    error_code = player_create(&ad->player0);
    if (error_code != PLAYER_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "failed to create player 0");
    error_code = player_create(&ad->player1);
    if (error_code != PLAYER_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "failed to create player 1");
    error_code = player_create(&ad->player2);
    if (error_code != PLAYER_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "failed to create player 2");
    error_code = player_create(&ad->player3);
    if (error_code != PLAYER_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "failed to create player 3");

    /*
       Perform more playback configuration, such as setting callbacks,
       setting the source file URI, and preparing the player
    */
    char *audio_path_0 = concat(app_get_shared_resource_path(), "pling.wav");
    char *audio_path_1 = concat(app_get_shared_resource_path(), "klack1.wav");
    char *audio_path_2 = concat(app_get_shared_resource_path(), "klack2.wav");
    char *audio_path_3 = concat(app_get_shared_resource_path(), "klack3.wav");

    error_code = player_set_uri(ad->player0, audio_path_0);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set URI for player 0: error code = %d", error_code);
    error_code = player_set_uri(ad->player1, audio_path_1);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set URI for player 1: error code = %d", error_code);
    error_code = player_set_uri(ad->player2, audio_path_2);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set URI for player 2: error code = %d", error_code);
    error_code = player_set_uri(ad->player3, audio_path_3);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to set URI for player 3: error code = %d", error_code);

	error_code = player_prepare(ad->player0);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to prepare player 0: error code = %d", error_code);
	error_code = player_prepare(ad->player1);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to prepare player 1: error code = %d", error_code);
	error_code = player_prepare(ad->player2);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to prepare player 2: error code = %d", error_code);
	error_code = player_prepare(ad->player3);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to prepare player 3: error code = %d", error_code);

	free(audio_path_0);
	free(audio_path_1);
	free(audio_path_2);
	free(audio_path_3);
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
    evas_object_smart_callback_add(ad->circle_slider, "value,changed", _circle_slider_value_changed_cb, ad);

	/* Bottom button */
	ad->button = elm_button_add(ad->layout);
	elm_object_text_set(ad->button, "Play");
	elm_object_style_set(ad->button, "bottom");
	evas_object_smart_callback_add(ad->button, "clicked", _bottom_button_clicked_cb, ad);
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

	init_base_player(ad);
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
	appdata_s *ad = data;
	int error_code = 0;

	/* Release all resources. */
	error_code = player_stop(ad->player0);
	error_code = player_unprepare(ad->player0);
	error_code = player_destroy(ad->player0);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "fail to destroy player 0: error code = %d", error_code);

	error_code = player_stop(ad->player1);
	error_code = player_unprepare(ad->player1);
	error_code = player_destroy(ad->player1);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "fail to destroy player 1: error code = %d", error_code);

	error_code = player_stop(ad->player2);
	error_code = player_unprepare(ad->player2);
	error_code = player_destroy(ad->player2);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "fail to destroy player 2: error code = %d", error_code);

	error_code = player_stop(ad->player3);
	error_code = player_unprepare(ad->player3);
	error_code = player_destroy(ad->player3);
	if (error_code != PLAYER_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "fail to destroy player 3: error code = %d", error_code);
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
