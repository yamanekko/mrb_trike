#include "ev3api.h"
#include "syssvc/serial.h"	// for Bluetooth(log)
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/string.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

/* メインタスク */
void main_task(intptr_t unused)
{
    /* LCD画面にログを表示 */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    ev3_lcd_draw_string("mruby Trike step 1", 0, CALIB_FONT_HEIGHT*1);

	static mrb_state *mrb = NULL;
	mrb_value ret;

	mrb = mrb_open();
	//mrb->code_fetch_hook = code_fetch;

	struct RClass * ev3rt = mrb_class_get(mrb, "EV3RT");
	mrb_define_const(mrb, ev3rt, "CONTROL_TASK_ID", mrb_fixnum_value(CONTROL_TASK));
	mrb_define_const(mrb, ev3rt, "WATCH_TASK_ID", mrb_fixnum_value(WATCH_TASK));
	mrb_define_const(mrb, ev3rt, "MAIN_TASK_ID", mrb_fixnum_value(MAIN_TASK));
	mrb_define_const(mrb, ev3rt, "CONTROL_CYC_ID", mrb_fixnum_value(CONTROL_CYC));
	mrb_define_const(mrb, ev3rt, "WATCH_CYC_ID", mrb_fixnum_value(WATCH_CYC));

    /* LCD画面にログを表示 */
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    ev3_lcd_draw_string("mruby Trike step 2", 0, CALIB_FONT_HEIGHT*1);

    #include "app_ruby.h"

    ret = mrb_load_irep (mrb, bcode);
    if(mrb->exc){
		if(!mrb_undef_p(ret)){
			ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
			ev3_lcd_draw_string("mruby Trike ERR", 0, CALIB_FONT_HEIGHT*1);
		    mrb_value s = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
		    if (mrb_string_p(s)) {
		    	ev3_lcd_draw_string(RSTRING_PTR(s), 0, CALIB_FONT_HEIGHT*3);
		    	serial_wri_dat(SIO_PORT_BT, RSTRING_PTR(s), RSTRING_LEN(s));
		     } else {
		    	 ev3_lcd_draw_string("error unknown", 0, CALIB_FONT_HEIGHT*3);
		     }
		 }
     }

    mrb_close(mrb);
    ext_tsk();
}

// 停止ボタン監視タスク
void cyclick_handler_status_check(intptr_t exinf) {
	wup_tsk(WATCH_TASK);
}

void watch_task(intptr_t exinf) {
    static mrb_state *mrb = NULL;
	mrb_value ret;

	mrb = mrb_open();

	struct RClass * ev3rt = mrb_class_get(mrb, "EV3RT");
	mrb_define_const(mrb, ev3rt, "CONTROL_TASK_ID", mrb_fixnum_value(CONTROL_TASK));
	mrb_define_const(mrb, ev3rt, "WATCH_TASK_ID", mrb_fixnum_value(WATCH_TASK));
	mrb_define_const(mrb, ev3rt, "MAIN_TASK_ID", mrb_fixnum_value(MAIN_TASK));
	mrb_define_const(mrb, ev3rt, "CONTROL_CYC_ID", mrb_fixnum_value(CONTROL_CYC));
	mrb_define_const(mrb, ev3rt, "WATCH_CYC_ID", mrb_fixnum_value(WATCH_CYC));

	#include "watch_ruby.h"
    ret = mrb_load_irep (mrb, watchcode);
    if(mrb->exc){
		if(!mrb_undef_p(ret)){
			ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
			ev3_lcd_draw_string("mruby Trike ERR", 0, CALIB_FONT_HEIGHT*1);
			mrb_value s = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
			if (mrb_string_p(s)) {
				ev3_lcd_draw_string(RSTRING_PTR(s), 0, CALIB_FONT_HEIGHT*3);
				serial_wri_dat(SIO_PORT_BT, RSTRING_PTR(s), RSTRING_LEN(s));
			} else {
				ev3_lcd_draw_string("error unknown", 0, CALIB_FONT_HEIGHT*3);
			}
		}
	}
	mrb_close(mrb);
}

// 走行制御タスク
void cyclick_handler(intptr_t exinf) {
	wup_tsk(CONTROL_TASK);
}

void control_tsk(intptr_t exinf) {
    static mrb_state *mrb = NULL;
	mrb_value ret;

	mrb = mrb_open();

	struct RClass * ev3rt = mrb_class_get(mrb, "EV3RT");
	mrb_define_const(mrb, ev3rt, "CONTROL_TASK_ID", mrb_fixnum_value(CONTROL_TASK));
	mrb_define_const(mrb, ev3rt, "WATCH_TASK_ID", mrb_fixnum_value(WATCH_TASK));
	mrb_define_const(mrb, ev3rt, "MAIN_TASK_ID", mrb_fixnum_value(MAIN_TASK));
	mrb_define_const(mrb, ev3rt, "CONTROL_CYC_ID", mrb_fixnum_value(CONTROL_CYC));
	mrb_define_const(mrb, ev3rt, "WATCH_CYC_ID", mrb_fixnum_value(WATCH_CYC));

	#include "control_ruby.h"
	ret = mrb_load_irep (mrb, ccode);
    if(mrb->exc){
		if(!mrb_undef_p(ret)){
			ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
			ev3_lcd_draw_string("mruby Trike ERR", 0, CALIB_FONT_HEIGHT*1);
		    mrb_value s = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
		    if (mrb_string_p(s)) {
		    	ev3_lcd_draw_string(RSTRING_PTR(s), 0, CALIB_FONT_HEIGHT*3);
		    	serial_wri_dat(SIO_PORT_BT, RSTRING_PTR(s), RSTRING_LEN(s));
			} else {
				ev3_lcd_draw_string("error unknown", 0, CALIB_FONT_HEIGHT*3);
			}
		}
	}
	mrb_close(mrb);
}

int _fini(void){
	return 0;
}
