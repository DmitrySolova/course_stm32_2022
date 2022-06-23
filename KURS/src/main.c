#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/timer.h>

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define FS 16000

void setup_dac();
void setup_adc();
uint16_t first_sample();
uint16_t adc();
void dac(uint16_t outputData);
void setup_timer (uint16_t Fs);
double IIRFilter(double x);

double v[4] = {0, 0, 0, 0};
int N = 5; // размер массива кэфов

const struct rcc_clock_scale* clocks = &rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_84MHZ];

// ===============================================

double b[] = {0.00319806247283222151, 0, -0.00639612494566444301, 0, 0.00319806247283222151};
double a[] = {1, -3.78125206442570505, 5.44829440482254412, -3.54414088511948755, 0.878834866871245235};

// ===============================================

int main () {

    // Разгон процессора
	rcc_clock_setup_pll(clocks);

	// тактирование портов A
	rcc_periph_clock_enable(RCC_GPIOA);

	setup_dac();
	setup_adc();
	setup_timer(FS);

    rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup (GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);


	uint16_t data_adc = first_sample();
	double data_filtered_double = 0;
	uint16_t data_filtered_12 = 0;

	while (1) {

		if (timer_get_flag(TIM6, TIM_SR_UIF)) {

 			gpio_set(GPIOD, GPIO15);
            
			data_adc = adc();

			data_filtered_double = IIRFilter(data_adc);
			data_filtered_12 = round(data_filtered_double) + 2047;

			dac(data_filtered_12);

			timer_clear_flag(TIM6, TIM_SR_UIF);
	
			while (! adc_eoc(ADC1));
 			gpio_clear(GPIOD, GPIO15);
		}

	}
}



// ===================== Цифровая фильтрация =====================

double IIRFilter(double x){
    double y = 0;   // выход
    int i = 0;
    y = v[0] + b[0]*x;

    // обновляем буфер для следующей итерации
    for (; i < N-2; i++)
        v[i] = v[i+1] + b[i+1]*x - a[i+1]*y;

    v[N-2] = b[N-1]*x - a[N-1]*y;

    return y;
}

// ===================== Инициализация ЦАП =====================

void setup_dac() {

	// Настройка порта A4 для выхода цифрового сигнала (осциллограф)
	gpio_mode_setup (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);

	// Тактирование ЦАП
	rcc_periph_clock_enable(RCC_DAC);

	// Задание канала ЦАП
	dac_enable(CHANNEL_1);
}

// ===================== Инициализация АЦП =====================

void setup_adc() {

	// Настройка порта A0 для входа аналогового сигнала (генератор)
	gpio_mode_setup (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);

	// Тактирование АЦП
	rcc_periph_clock_enable(RCC_ADC1);

	// Переход в режим одноканального сканирования каналов
	adc_disable_scan_mode(ADC1);

	// Одиночное преобразование
	adc_set_single_conversion_mode(ADC1);

	// Тактирования блока преобразователя УВХ, примерно 15 тактов (в тактах АЦП) 
	adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_15CYC);

	// Подаем опорное напряжение на внутренний АЦП
	adc_power_on(ADC1); 

}

// ===================== Снятие первого отсчета с АЦП, для правильной работы =====================

uint16_t first_sample() {
	
	// Запуск одиночного преобразования
	adc_start_conversion_regular(ADC1);
	
	// Блокирующий оператор - появился результат АЦП
	while (! adc_eoc(ADC1));

	// Записываем результат с АЦП
	uint16_t adc_retult = adc_read_regular(ADC1);

	// Запуск следующего преобразования
	adc_start_conversion_regular(ADC1);	
	
	return adc_retult;
}

// ===================== Снятие данных с АЦП =====================

uint16_t adc() {

	// Записываем результат с АЦП
	uint16_t temp_data_from_adc = adc_read_regular(ADC1);
	
	// Запуск следующего преобразования
	adc_start_conversion_regular(ADC1);

	return temp_data_from_adc;
}

// ===================== Передача отфильтрованных данных на ЦАП =====================

void dac(uint16_t outputData) {

	dac_load_data_buffer_single(outputData, RIGHT12, CHANNEL_1 );

}

// ===================== Настройка таймера =====================

void setup_timer (uint16_t Fs) {
	
	rcc_periph_clock_enable(RCC_TIM6);

	timer_set_prescaler(TIM6, 1 - 1);

	timer_set_period(TIM6, 5250 - 1);

	timer_enable_irq(TIM6, TIM_DIER_UIE);

	timer_enable_counter(TIM6);

}
