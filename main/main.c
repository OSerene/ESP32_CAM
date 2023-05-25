/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"


#include "myimg.h"
#include "page_start.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "bsp_adc.h"
#include "button.h"


/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    #if defined CONFIG_LV_USE_DEMO_WIDGETS
        #include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
    #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #include "lv_examples/src/lv_demo_keypad_encoder/lv_demo_keypad_encoder.h"
    #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
        #include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
    #elif defined CONFIG_LV_USE_DEMO_STRESS
        #include "lv_examples/src/lv_demo_stress/lv_demo_stress.h"
    #else
        #error "No demo application selected."
    #endif
#endif

/*********************
 *      DEFINES
 *********************/
#define TAG "demo"
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);

// static void SPIFFS_Directory(char *path)
// {
// 	DIR *dir = opendir(path);
// 	assert(dir != NULL);
// 	while (true)
// 	{
// 		struct dirent *pe = readdir(dir);
// 		if (!pe)
// 			break;
// 		ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
// 	}
// 	closedir(dir);
// }

void page_switch()
{
	uint32_t user_data;
	user_data = Button_Value;

#if 0
    if (Button_Value == BT1_LONG || Button_Value == BT3_LONG)
    {
        
    }
    else
    {
        Button_Value = 0;
    }

    if (user_data == BT1_DOWN || user_data == BT3_DOWN)
    {
        switch (Disp)
        {
        case Disp_Home:
            move_task_home(user_data);
            break;
        case Disp_Menu:
            move_task_menu(user_data);
            break;
        case Disp_Cam:
            // move_task_cam(user_data);
            break;
        case Disp_Color:
            move_task_color(user_data);
            break;
        case Disp_Face:
            ((user_data == BT1_DOWN) ? (g_state = START_DELETE) : (g_state = START_RECOGNITION));
            // move_task_cam(user_data);
            break;
        case Disp_Baidu:
            move_task_baiduai(user_data);
            break;
        case Disp_FFT:
            move_task_fft(user_data);
            break;
        case Disp_Daily:
            move_task_daily(user_data);
            break;
        case Disp_Game_2048:
            move_task_game_2048(user_data);
            break;
        case Disp_Game_Snake:
            move_task_game_snake(user_data);
            break;
        case Disp_Calendar:
            // move_task_calendar(user_data);
            encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
            break;
        case Disp_Setting:
            encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
            // move_task_calendar(user_data);
            break;
        default:
            break;
        }
    }
    else if (user_data == BT1_LONG || user_data == BT3_LONG)
    {
        switch (Disp)
        {
        case Disp_Home:
            // move_task_home(user_data);
            break;
        case Disp_Menu:

            move_task_menu(user_data);
            break;
        case Disp_Cam:
            // move_task_cam(user_data);
            break;
        case Disp_Color:
            // move_task_color(user_data);
            break;
        case Disp_Face:
            // ((user_data == BT1_DOWN) ? (g_state = START_DELETE) : (g_state = START_RECOGNITION));
            // move_task_cam(user_data);
            break;
        case Disp_Baidu:

            // move_task_baiduai(user_data);
            break;
        case Disp_Daily:
            move_task_daily(user_data);
            break;

        case Disp_Calendar:
            // move_task_calendar(user_data);
            // encoder_handler((user_data == BT1_DOWN) ? 2 : 3);
            break;
        case Disp_Setting:
            encoder_handler((user_data == BT1_LONG) ? 2 : 3);
            // move_task_calendar(user_data);
            break;
        default:
            break;
        }
    }
    else if (user_data == BT2_DOWN)
    {
        switch (Disp)
        {
        case Disp_Home:
            // move_task_home(user_data);

            break;
        case Disp_Menu:
            page_menu_end();
            page_manage[Menu_Choose()].start();
            Disp = Menu_Choose();
            switch (Menu_Choose())
            {
            case Disp_Cam:
                cam_en = 1;
                break;
            case Disp_Color:
                cam_en = 1;
                color_en = 1;
                break;
            case Disp_Face:
                face_en = 1;
                g_state = START_RECOGNITION;
                xTaskCreatePinnedToCore(&Face_DEC, "Face_DEC", 1024 * 4, NULL, 5, NULL, 0);
                break;
            case Disp_Baidu:
                baiduai_en = 1;
                break;
            case Disp_FFT:
                fft_en = 1;
                xTaskCreatePinnedToCore(&FFT_Task, "FFT_Task", 1024 * 8, NULL, 6, NULL, 0);
                break;
            case Disp_About:

                break;

            default:
                break;
            }
            break;
        case Disp_Face:
            // move_task_cam(user_data);
            g_state = START_ENROLL;
            break;
        case Disp_Daily:
            move_task_daily(user_data);
            break;
        case Disp_Calendar:
            // move_task_calendar(user_data);
            encoder_handler(1);
            break;
        case Disp_Setting:
            encoder_handler(1);
            // move_task_calendar(user_data);
            break;
        case Disp_Baidu:
            move_task_baiduai(user_data);
            break;
        default:
            break;
        }
    }
    else if (user_data == BT1_DOUBLE || user_data == BT3_DOUBLE)
    {
        switch (Disp)
        {
        case Disp_Game_2048:
            move_task_game_2048(user_data);
            break;
        case Disp_Game_Snake:
            move_task_game_snake(user_data);
            break;
            default:
            break;
            // if (Disp != Disp_Menu && Disp != Disp_Home)
            // {
            // 	page_manage[Menu_Choose()].end();
            // 	page_menu_start();
            // 	Disp = Disp_Menu;
            // }
        }
    }
    else if (user_data == BT2_LONG)
    {
        if (Disp == Disp_Menu || Disp == Disp_Home)
        {
            static uint8_t set = 1;
            if (set)
            {
                ksdiy_sys_t.state.sys_button = 1; //使用lvgl按键机制
                page_home_end();
                page_menu_start();
                Disp = Disp_Menu;
                set = 0;
            }
            else
            {
                ksdiy_sys_t.state.sys_button = 0; //退出lvgl按键机制
                page_menu_end();
                page_home_start();
                Disp = Disp_Home;
                set = 1;
            }
        }
        else
        {
            cam_en = 0, color_en = 0, face_en = 0;
            page_manage[Menu_Choose()].end();
            page_menu_start();
            Disp = Disp_Menu;
        }
    }
    printf("Disp: %d\n", Disp);
#endif
}

void button_task(void *arg)
{
	Button_Init();
	while (1)
	{
		Button_Process();
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}
}

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main() {

    ESP_LOGI(TAG, "Initializing SPIFFS");
	// spiffs_driver_init();
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 8,
		.format_if_mount_failed = true};
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK)
	{
		if (ret == ESP_FAIL)
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		else if (ret == ESP_ERR_NOT_FOUND)
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		else
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		return;
	}
    else
    {
        ESP_LOGE(TAG, "success to mount or format filesystem");
    }
	// SPIFFS_Directory("/spiffs/");

    // Initialize NVS
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);

    adc_init();
	// ESP_LOGI("esp-eye", "Please say 'Hi LeXin' to the board");
	// ESP_LOGI("esp-eye", "Version " VERSION);
	xTaskCreatePinnedToCore(&button_task, "button_task", 1024 * 2, NULL, 18, NULL, 0);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    // create_demo_application();
    page_init();

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
        page_switch();
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}

static void create_demo_application(void)
{
    /* When using a monochrome display we only show "Hello World" centered on the
     * screen */
#if defined CONFIG_LV_TFT_DISPLAY_MONOCHROME || \
    defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7735S

    /* use a pretty small demo for monochrome displays */
    /* Get the current screen  */
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);

    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(scr, NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello\nworld");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
#else
    /* Otherwise we show the selected demo */

    #if defined CONFIG_LV_USE_DEMO_WIDGETS
        lv_demo_widgets();
    #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        lv_demo_keypad_encoder();
    #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
        lv_demo_benchmark();
    #elif defined CONFIG_LV_USE_DEMO_STRESS
        lv_demo_stress();
    #else
        #error "No demo application selected."
    #endif
#endif
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
