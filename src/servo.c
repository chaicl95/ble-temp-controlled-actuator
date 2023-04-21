#include <servo.h>

mcpwm_timer_handle_t mcpwm_timer = NULL;
mcpwm_timer_config_t mcpwm_timer_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
    .period_ticks = SERVO_TIMEBASE_PERIOD,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
};
mcpwm_oper_handle_t mcpwm_oper = NULL;
mcpwm_operator_config_t mcpwm_oper_config = {
    .group_id = 0, // operator must be in the same group to the timer
};
mcpwm_cmpr_handle_t mcpwm_cmpr = NULL;
mcpwm_comparator_config_t mcpwm_cmpr_config = {
    .flags.update_cmp_on_tez = true,
};
mcpwm_gen_handle_t mcpwm_gen = NULL;
mcpwm_generator_config_t mcpwm_gen_config = {
    .gen_gpio_num = SERVO_PULSE_GPIO,
};

void pwm_init(void) {
    ESP_ERROR_CHECK(mcpwm_new_timer(&mcpwm_timer_config, &mcpwm_timer));
    ESP_ERROR_CHECK(mcpwm_new_operator(&mcpwm_oper_config, &mcpwm_oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(mcpwm_oper, mcpwm_timer));
    ESP_ERROR_CHECK(mcpwm_new_comparator(mcpwm_oper, &mcpwm_cmpr_config, &mcpwm_cmpr));
    ESP_ERROR_CHECK(mcpwm_new_generator(mcpwm_oper, &mcpwm_gen_config, &mcpwm_gen));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr, angle_to_compare(0)));
    // go high on counter empty
    ESP_ERROR_CHECK(
        mcpwm_generator_set_actions_on_timer_event(
            mcpwm_gen, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(
        mcpwm_generator_set_actions_on_compare_event(
            mcpwm_gen, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_cmpr, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_timer_enable(mcpwm_timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(mcpwm_timer, MCPWM_TIMER_START_NO_STOP));
}