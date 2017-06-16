#include "nrf_drv_adc.h"
#include "app_error.h"


static nrf_drv_adc_channel_t m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_7); /**< Channel instance. Default configuration used. */
static nrf_adc_value_t adc_buf[1];

#define ADC_REF_VBG_VOLTAGE_IN_MILLIVOLTS 1200   
#define ADC_RES_10BIT                     1024    
#define ADC_INPUT_PRESCALER               3

#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
			((((ADC_VALUE) * ADC_REF_VBG_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_INPUT_PRESCALER)
			
				
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
			
				uint16_t        batt_lvl_in_milli_volts;
				nrf_adc_value_t adc_result;
				uint32_t        err_code;
				
				adc_result = p_event->data.done.p_buffer[0];
			
				err_code = nrf_drv_adc_buffer_convert(p_event->data.done.p_buffer, 1);	
				APP_ERROR_CHECK(err_code);
			
				batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result);// +
                                 // DIODE_FWD_VOLT_DROP_MILLIVOLTS;
				printf("v:%d\r\n",batt_lvl_in_milli_volts);
				
    }
}



void adc_config_init(void)
{
    ret_code_t ret_code;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;

    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);

    nrf_drv_adc_channel_enable(&m_channel_config);
	
		nrf_drv_adc_buffer_convert(&adc_buf[0], 1);
}


void start_read_adc(void)
{
    nrf_drv_adc_sample();
}



